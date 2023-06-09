^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package canopen_core
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Forthcoming
-----------
* Reduce processor load (`#111 <https://github.com/ros-industrial/ros2_canopen/issues/111>`_)
  * Get slave eds and bin in node_canopen_driver
  * Add dictionary to base driver
  * Enable dictionary in proxy drivers
  * Add a few test objects
  * Add pdo checks
  * Adjust 402 driver
  * Fix tests
  * rename to get_xx_queue
  * Add typed sdo operations
  * Remove object datatype where possible
  * Add plain operation mode setting + switchingstate
  * Add robot system interface
  * Add robot system controller
  * Add robot_system_tests
  * Add a bit of documentation
  * Add in code documentation
  * Fix bug
  * Add examples section
  * Fix set_target for interpolated mode
  * Switch to rclcpp::sleep_for
  * Fix initialization for state and command interface variables
  * Add remade robot system interfce
  * Add copyright info
  * Fix missing return statement
  * processing behavior improvement
  * Minor changes to make things work
  * Add poll_timer_callback
  * Fix format
  * Add polling mode variable for config.
  ---------
  Co-authored-by: Vishnuprasad Prachandabhanu <vishnu.pbhat93@gmail.com>
* Remove type indication from msg and srv interfaces (`#112 <https://github.com/ros-industrial/ros2_canopen/issues/112>`_)
  * Get slave eds and bin in node_canopen_driver
  * Add dictionary to base driver
  * Enable dictionary in proxy drivers
  * Add a few test objects
  * Add pdo checks
  * Adjust 402 driver
  * Fix tests
  * rename to get_xx_queue
  * Add typed sdo operations
  * Remove object datatype where possible
  ---------
* Add driver dictionaries (`#110 <https://github.com/ros-industrial/ros2_canopen/issues/110>`_)
  * Get slave eds and bin in node_canopen_driver
  * Add dictionary to base driver
  * Enable dictionary in proxy drivers
  * Add a few test objects
  * Add pdo checks
  * Adjust 402 driver
  * Fix tests
  * rename to get_xx_queue
  * Add typed sdo operations
  ---------
* Implemented thread-safe queue for rpdo and emcy listener (`#97 <https://github.com/ros-industrial/ros2_canopen/issues/97>`_)
  * Boost lock free queue implemetation
  * include boost libraries in CMakelists
  * Testing rpdo/tpdo ping pond
  * pre-commit changes
  * Bugfix: implemented timeout for wait_and_pop to avoid thread blocking
  * Fixed typo
  * pre-commit update
  * FIxed: properly export Boost libraries
  * Update code documentation
* Add EMCY callback to base driver (`#91 <https://github.com/ros-industrial/ros2_canopen/issues/91>`_)
* Better organize dependencies (`#88 <https://github.com/ros-industrial/ros2_canopen/issues/88>`_)
* Merge branch 'master' into patch-2
* Merge branch 'bjsowa-master'
* Don't treat options as driver
* Merge branch 'master' of github.com:bjsowa/ros2_canopen into bjsowa-master
* Don't treat options section as another device
* Merge remote-tracking branch 'ros/master'
* Precommit changes (`#79 <https://github.com/ros-industrial/ros2_canopen/issues/79>`_)
  * Precommit changes
  * Update to clang-format-14
* Disable device container tests (`#77 <https://github.com/ros-industrial/ros2_canopen/issues/77>`_)
* Handle demand set master failure (`#70 <https://github.com/ros-industrial/ros2_canopen/issues/70>`_)
  * adapt fake cia402 slave
  * Add retries for demand_set_master in case of failure
* Doxygen documentation for canopen_core (`#78 <https://github.com/ros-industrial/ros2_canopen/issues/78>`_)
  * canopen_core in code documentation
  * Some more documentation
* intra_process_comms
* Doxygen documentation for canopen_core (`#78 <https://github.com/ros-industrial/ros2_canopen/issues/78>`_)
  * canopen_core in code documentation
  * Some more documentation
* intra_process_comms
* Add unit tests for canopen_core (`#64 <https://github.com/ros-industrial/ros2_canopen/issues/64>`_)
  * Testing changes to canopen_core
  * Testing changes to canopen_base_driver and canopen_402_driver
  * Add canopen_core tests (90% coverage)
  * Fix DriverException error in canopen_402_driver
  * Catch errors in nmt and rpdo listeners
  * Fix naming issues
  * Fix deactivate transition
  * Fix unclean shutdown
* Merge pull request `#60 <https://github.com/ros-industrial/ros2_canopen/issues/60>`_ from ipa-cmh/merge-non-lifecycle-and-lifecycle-drivers
  Streamline driver and master infrastructure
* undo renaming can_interface_name -> can_interface
* Undo formatting in ros2_control
* Add lifecycle manager to device_container library
* Remove canopen_lifecycle.launch.py as it i no longer needed.
* Streamline logging
* Integrate lifecycle manager
* Fix tests canopen_core
* Fix canopen_master_driver for explicit instantiation
* Add CanopenDriverInterface Documentation
* Add master dcfs and remove from gitignore
* add node interface accessor  and lifecycle information to drivers
* Add type accessor functions to device_container
* Integration with ros2_control
* Add function to device container
* Add device container and general changes to make things work.
* Update CmakeFile of canopen core
* Add tests to canopen core
* Remove device and do some renaming
* Add node base classes
* Add errors
* Add core node interfaces
* Add in code documentation for canopen_core (`#53 <https://github.com/ros-industrial/ros2_canopen/issues/53>`_)
  * Document device container node
  * Document lely_master_bridge
  * Document Lifecycle Device Container
  * Document Lifecycle Device Manager
  * Document LifecyleMasterNode
  * Document Master Node
  * Fix error
  * Document lifecycle base driver
  * Document lely bridge
  * Document canopen_proxy_driver
  * Document canopen_402_driver
* Add configuration parameter passthrough (`#52 <https://github.com/ros-industrial/ros2_canopen/issues/52>`_)
* Publish joint state instead of velocity topics (`#47 <https://github.com/ros-industrial/ros2_canopen/issues/47>`_)
  * disable loader service
  * add custom target/command and install to macro
  * publish jointstate
  * correct variable name squiggle
  * Minor changes to driver and slave
  * Update lely core library
  * Add sensor_msgs to dependencies
  * Remove artifacts
  * Remove some artifacts
* Disable dynamic loading for containers (`#50 <https://github.com/ros-industrial/ros2_canopen/issues/50>`_)
  * disable loader service
  * Remove artifacts
* Merge pull request `#33 <https://github.com/ros-industrial/ros2_canopen/issues/33>`_ from StoglRobotics-forks/canopen-system-interface
  Add generic system interface for ros2_control
* Update canopen_core/CMakeLists.txt
* Fix merging issues.
* Merge remote-tracking branch 'livanov/fix-dependencies' into canopen-system-interface
* Remove some unecessary changes.
* Apply suggestions from code review
* Add nmt and rpdo callbacks.
* Start device manager in system interface.
* fix-ifndef-directive (`#43 <https://github.com/ros-industrial/ros2_canopen/issues/43>`_)
* Add lifecycle to service-based operation (`#34 <https://github.com/ros-industrial/ros2_canopen/issues/34>`_)
  * Add check if remote object already exists to avoid multiple objects with same target.
  * Renaming and changes to MasterNode
  * restrucutring for lifecycle support
  * changes to build
  * Add lifecycle to drivers, masters and add device manager
  * Add lifecycled operation canopen_core
  * Added non lifecycle stuff to canopen_core
  * Add lifecyle to canopen_base_driver
  * Add lifecycle to canopen_proxy_driver
  * restructured canopen_core for lifecycle support
  * restructured canopen_base_driver for lifecycle support
  * Restrucutured canopen_proxy_driver for lifecycle support
  * Restructured canopen_402_driver for lifecycle support
  * Add canopen_mock_slave add cia402 slave
  * add canopen_tests package for testing canopen stack
  * Disable linting for the moment and some foxy compat changes
  * Further changes for foxy compatability
* Merge pull request `#1 <https://github.com/ros-industrial/ros2_canopen/issues/1>`_ from livanov93/canopen-system-interface
  [WIP] Add ros2_control system interface wrapper for ros2_canopen functionalities
* Apply suggestions from code review
* Remove pedantic cmake flags.
* Add nmt and rpdo callbacks.
* Start device manager in system interface.
* Add yaml_cpp_vendor as dependency for master_node (`#18 <https://github.com/ros-industrial/ros2_canopen/issues/18>`_)
  * commented out the yaml_cpp vendor since it doesnt let it build on the 20.04 machine
  * Update CMakeLists.txt
  * Update CMakeLists.txt
  * Update CMakeLists.txt
  Co-authored-by: Christoph Hellmann Santos <51296695+ipa-cmh@users.noreply.github.com>
* Configuration manager integration (`#14 <https://github.com/ros-industrial/ros2_canopen/issues/14>`_)
  * Add longer startup delay and test documentation
  * Add speed and position publisher
  * Create Configuration Manager
  * make MasterNode a component and add configuration manager functionalities
  * add configuration manager functionalities
  * add configuration manger functionalities
  * Add documentation for Configuration Manager
  * add info messages and documentation
  * update launch files and configuration fiels
  * add can_utils package
  * add info text
  * simplify dependencies
  * remove tests from can_utils
  * avoid tests for canopen_utils
  * changes info logging and adds nmt and sdo tests
  * add tests
  * remove launch_tests from cmake
* Add Industrial CI for foxy and galactic (`#10 <https://github.com/ros-industrial/ros2_canopen/issues/10>`_)
  * Add ci
  * remove rclcpp_lifecycle stuff from CMAKElists.txt
  * add rclcpp_lifecycle and msgs
  * add galactic to ci and limit ci to prs and pushes to master
* Merge branch 'licenses' into 'master'
  add licenses to each package
  See merge request ipa326/ros-industrial/ros2_canopen!22
* add header guards
* update package descriptions
* add license in files
* Merge branch 'renaming' into 'master'
  Update package names to fit ROS2 naming rules better
  See merge request ipa326/ros-industrial/ros2_canopen!21
* store tests of proxy driver in canopen_proxy_driver
* rename packages to fit ROS2 conventions better
* Contributors: Aulon Bajrami, Borong Yuan, Błażej Sowa, Christoph Hellmann Santos, Denis Štogl, Lovro, Vishnuprasad Prachandabhanu