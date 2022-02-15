//    Copyright 2022 Christoph Hellmann Santos
// 
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
#ifndef ROS2_CANOPEN_NODE_HPP
#define ROS2_CANOPEN_NODE_HPP

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <utility>
#include <fstream>
#include <map>

#include <lely/ev/loop.hpp>
#include <lely/io2/linux/can.hpp>
#include <lely/io2/posix/poll.hpp>
#include <lely/io2/sys/io.hpp>
#include <lely/io2/sys/timer.hpp>
#include <lely/coapp/master.hpp>
#include <lely/coapp/fiber_driver.hpp>
#include <lely/ev/co_task.hpp>

#include <yaml-cpp/yaml.h>

#include "rclcpp/macros.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "lifecycle_msgs/srv/change_state.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "std_msgs/msg/string.hpp"
#include "pluginlib/class_loader.hpp"

#include "ros2_canopen_interfaces/srv/co_nmt_id.hpp"
#include "ros2_canopen_interfaces/srv/co_read_id.hpp"
#include "ros2_canopen_interfaces/srv/co_heartbeat_id.hpp"
#include "ros2_canopen_interfaces/srv/co_write_id.hpp"
#include "ros2_canopen_core/proxy_device.hpp"

using namespace std::chrono_literals;
using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;
using namespace lely;

namespace ros2_canopen
{
    /**
     * @brief CANopen Master Node
     * 
     * This class provides a CANopen Master Node.
     * 
     */
    class CANopenNode : public rclcpp_lifecycle::LifecycleNode
    {
    private:
        rclcpp::TimerBase::SharedPtr timer_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
        std::shared_ptr<io::IoGuard> io_guard;
        std::shared_ptr<io::Context> ctx;
        std::shared_ptr<io::Poll> poll;
        std::shared_ptr<ev::Loop> loop;
        std::shared_ptr<ev::Executor> exec;
        std::shared_ptr<io::CanController> ctrl;
        std::shared_ptr<io::CanChannel> chan;
        std::shared_ptr<io::Timer> can_timer;
        std::shared_ptr<canopen::AsyncMaster> can_master;
        std::shared_ptr<ros2_canopen::ProxyDevice> basicdevice;
        std::shared_ptr<std::string> nmt_status;

        std::string can_interface_name;
        std::string dcf_path;
        std::string yaml_path;
        std::map<int, std::string> drivers;
        std::shared_ptr<std::map<int, std::shared_ptr<ros2_canopen::CANopenDevice>>> devices;
        std::unique_ptr<std::thread> canopen_loop_thread;

        std::shared_ptr<rclcpp::Service<ros2_canopen_interfaces::srv::CONmtID>> master_nmt_service;
        std::shared_ptr<rclcpp::Service<ros2_canopen_interfaces::srv::COReadID>> master_read_sdo_service;
        std::shared_ptr<rclcpp::Service<ros2_canopen_interfaces::srv::COHeartbeatID>> master_set_hearbeat_service;
        std::shared_ptr<rclcpp::Service<ros2_canopen_interfaces::srv::COWriteID>> master_write_sdo_service;

        std::atomic<bool> active;
        std::atomic<bool> configured;
        std::shared_ptr<std::mutex> master_mutex;
        
        std::promise<void> post_registration;
        std::future<void> post_registration_done;
        std::promise<void> pre_deregistration;
        std::future<void> pre_deregistration_done;
        std::promise<void> registration_done;
        std::promise<void> active_p;
        std::future<void> master_thread_running;
        std::mutex lifecycle_mutex;



        //Service Callback Declarations
        void master_nmt(
            const std::shared_ptr<ros2_canopen_interfaces::srv::CONmtID::Request> request,
            std::shared_ptr<ros2_canopen_interfaces::srv::CONmtID::Response> response);

        void master_read_sdo(
            const std::shared_ptr<ros2_canopen_interfaces::srv::COReadID::Request> request,
            std::shared_ptr<ros2_canopen_interfaces::srv::COReadID::Response> response);

        void master_set_heartbeat(
            const std::shared_ptr<ros2_canopen_interfaces::srv::COHeartbeatID::Request> request,
            std::shared_ptr<ros2_canopen_interfaces::srv::COHeartbeatID::Response> response);

        void master_write_sdo(
            const std::shared_ptr<ros2_canopen_interfaces::srv::COWriteID::Request> request,
            std::shared_ptr<ros2_canopen_interfaces::srv::COWriteID::Response> response);


        //helper functions
        void run();
        void read_yaml();
        void register_services();
        void register_drivers();
        void deregister_drivers();
        CallbackReturn change_state(
            const std::uint8_t transition, 
            std::chrono::seconds time_out = 4s);


        // Tasks
        class WriteSdoCallbackCoTask : public ev::CoTask
        {
        private:
            std::promise<bool> prom;
            ev::Future<void, std::exception_ptr> f;

        public:
            WriteSdoCallbackCoTask(ev_exec_t *exec) : CoTask(exec)
            {
            }
            void set_lely_future(ev::Future<void, std::exception_ptr> fut)
            {
                this->f = fut;
            }
            std::future<bool> get_future()
            {
                return prom.get_future();
            }
            void operator()() noexcept
            {
                if (!f.get().has_error())
                {
                    prom.set_value(true);
                }
                else
                {
                    prom.set_exception(f.get().error());
                }
            }
        };

        template <typename T>
        class WriteSdoCoTask : public ev::CoTask
        {
        private:
            uint16_t index;
            uint8_t nodeid;
            uint8_t subindex;
            T data;
            std::unique_ptr<WriteSdoCallbackCoTask> callback_task;
            std::shared_ptr<canopen::AsyncMaster> can_master;

        public:
            WriteSdoCoTask(ev_exec_t *exec) : CoTask(exec)
            {
                callback_task = std::make_unique<WriteSdoCallbackCoTask>(exec);
            }
            void set_data(std::shared_ptr<canopen::AsyncMaster> can_master, uint8_t nodeid, uint16_t index, uint8_t subindex, T data)
            {
                this->index = index;
                this->nodeid = nodeid;
                this->subindex = subindex;
                this->data = data;
                this->can_master = can_master;
            }

            std::future<bool> get_future()
            {

                return callback_task->get_future();
            }

            void operator()() noexcept
            {
                auto f = can_master->AsyncWrite<T>(this->exec, nodeid, index, subindex, std::move(data), 100ms);
                callback_task->set_lely_future(f);
                //set callback task to be executed when future is ready
                f.submit(*callback_task);
            }
        };

        template <typename T>
        class ReadSdoCallbackCoTask : public ev::CoTask
        {
        private:
            std::promise<T> prom;
            ev::Future<T, std::exception_ptr> f;

        public:
            ReadSdoCallbackCoTask(ev_exec_t *exec) : CoTask(exec)
            {
            }
            void set_lely_future(ev::Future<T, std::exception_ptr> fut)
            {
                this->f = fut;
            }
            std::future<T> get_future()
            {
                return prom.get_future();
            }
            void operator()() noexcept
            {
                if (!f.get().has_error())
                {
                    prom.set_value(f.get().value());
                }
                else
                {
                    prom.set_exception(f.get().error());
                }
            }
        };

        template <typename T>
        class ReadSdoCoTask : public ev::CoTask
        {
        private:
            uint16_t index;
            uint8_t nodeid;
            uint8_t subindex;
            std::unique_ptr<ReadSdoCallbackCoTask<T>> callback_task;
            std::shared_ptr<canopen::AsyncMaster> can_master;

        public:
            ReadSdoCoTask(ev_exec_t *exec) : CoTask(exec)
            {
                callback_task = std::make_unique<ReadSdoCallbackCoTask<T>>(exec);
            }
            void set_data(std::shared_ptr<canopen::AsyncMaster> can_master, uint8_t nodeid, uint16_t index, uint8_t subindex)
            {
                this->index = index;
                this->nodeid = nodeid;
                this->subindex = subindex;
                this->can_master = can_master;
            }

            std::future<T> get_future()
            {

                return callback_task->get_future();
            }

            void operator()() noexcept
            {
                auto f = can_master->AsyncRead<T>(this->exec, nodeid, index, subindex, 100ms);
                callback_task->set_lely_future(f);
                //set callback task to be executed when future is ready
                f.submit(*callback_task);
            }
        };

        template <typename T>
        void master_read(
            const std::shared_ptr<ros2_canopen_interfaces::srv::COReadID::Request> request,
            std::shared_ptr<ros2_canopen_interfaces::srv::COReadID::Response> response)
        {
            if (active.load())
            {
                ReadSdoCoTask<T> read_task(*exec);
                read_task.set_data(can_master, request->nodeid, request->index, request->subindex);
                auto f = read_task.get_future();
                {
                    //get lock on master and the canopen executor
                    std::scoped_lock<std::mutex> lk(*master_mutex);
                    //append task to read Sdo
                    exec->post(read_task);
                }
                f.wait();
                try
                {
                    response->data = (uint32_t)f.get();
                    response->success = true;
                }
                catch (std::exception &e)
                {
                    RCLCPP_ERROR(this->get_logger(), e.what());
                    response->success = false;
                }
            }
            else
            {
                RCLCPP_ERROR(this->get_logger(), "Couldn't write SDO because node not active");
                response->success = false;
            }
        }

        template <typename T>
        void master_write(
            const std::shared_ptr<ros2_canopen_interfaces::srv::COWriteID::Request> request,
            std::shared_ptr<ros2_canopen_interfaces::srv::COWriteID::Response> response)
        {
            if (active.load())
            {
                WriteSdoCoTask<T> write_task(*exec);
                write_task.set_data(can_master, request->nodeid, request->index, request->subindex, static_cast<T>(request->data));
                auto f = write_task.get_future();
                {
                    //get lock on master and the canopen executor
                    std::scoped_lock<std::mutex> lk(*master_mutex);
                    exec->post(write_task);
                }
                f.wait();
                try
                {
                    response->success = f.get();
                }
                catch (std::exception &e)
                {
                    RCLCPP_ERROR(this->get_logger(), e.what());
                    response->success = false;
                }
            }
            else
            {
                RCLCPP_ERROR(this->get_logger(), "Couldn't write SDO because node not active");
                response->success = false;
            }
        }
        //Lifecycle Callback Functions
        CallbackReturn on_configure(const rclcpp_lifecycle::State &state);
        CallbackReturn on_activate(const rclcpp_lifecycle::State &state);
        CallbackReturn on_deactivate(const rclcpp_lifecycle::State &state);
        CallbackReturn on_cleanup(const rclcpp_lifecycle::State &state);
        CallbackReturn on_shutdown(const rclcpp_lifecycle::State &state);
        pluginlib::ClassLoader<ros2_canopen::CANopenDevice> poly_loader;
    public:
        /**
         * @brief Construct a new CANopenNode object
         * 
         * @param node_name 
         * @param intra_process_comms 
         */
        CANopenNode(const std::string &node_name, bool intra_process_comms = false) 
        : rclcpp_lifecycle::LifecycleNode(
                node_name,
                rclcpp::NodeOptions().use_intra_process_comms(intra_process_comms)),
          poly_loader("ros2_canopen_core", "ros2_canopen::CANopenDevice")
        {
            this->declare_parameter<std::string>("can_interface_name", "vcan0");
            this->declare_parameter<std::string>("dcf_path", "");
            this->declare_parameter<std::string>("yaml_path", "");
            can_interface_name = "";
            dcf_path = "";
            yaml_path = "";
            this->register_services();
            this->master_mutex = std::make_shared<std::mutex>();
            this->devices = std::make_shared<std::map<int, std::shared_ptr<ros2_canopen::CANopenDevice>>>();
        }

        /**
         * @brief Gets Post Registration Future
         * 
         * @param post_done_f 
         * @return std::future<void> 
         * Is set once drivers are registered.
         */
        std::future<void> get_post_registration_future(std::future<void>&& post_done_f){
            post_registration = std::promise<void>();
            post_registration_done = std::forward<std::future<void>>(post_done_f);
            return post_registration.get_future();
        }

        /**
         * @brief Gets Pre-Deregistration Future
         * 
         * @param pre_dereg_f 
         * @return std::future<void> 
         * Is set before drivers are deregistered.
         */
        std::future<void> get_pre_deregistration_future(std::future<void>&& pre_dereg_f){
            pre_deregistration = std::promise<void>();
            pre_deregistration_done = std::forward<std::future<void>>(pre_dereg_f);
            return pre_deregistration.get_future();
        }

        /**
         * @brief Gets a Map of all registered drivers
         * 
         * @return std::shared_ptr<std::map<int, std::shared_ptr<ros2_canopen::CANopenDevice>>> 
         */
        std::shared_ptr<std::map<int, std::shared_ptr<ros2_canopen::CANopenDevice>>>
        get_driver_nodes(){
            return this->devices;
        }
    };
}

#endif