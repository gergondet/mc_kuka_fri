mc_kuka_fri
==

[![License](https://img.shields.io/badge/License-BSD%202--Clause-green.svg)](https://opensource.org/licenses/BSD-2-Clause)

Interface between [kuka_fri] and [mc_rtc]. It provides multi-robot support.

It also provides two robot modules:
- iiwa7
- iiwa14

Dependencies
------------

This package requires:
- [mc_rtc]
- [mesh_sampling]
- [iiwa_description]
- [kuka_fri]

This package can be built with [kuka_fri]. To do so, pass the `-DWITH_KUKA_FRI=OFF` when configuring the package. In such case, only the robot modules will be provided.

Usage
--

1. Install this project's dependencies
2. Install this project (`cmake`/`make`/`make install`)

Your mc_rtc configuration file (typically `~/.config/mc_rtc/mc_rtc.yaml`) should contain the following lines:

```yaml
# General mc_rtc configuration to run a panda controller at 1kHz
MainRobot: iiwa14 # Or iiwa7
Enabled: YourController
# Timestep of the controller that will also be used for the Kuka arm(s)
Timestep: 0.005

# Set a LogPolicy suitable for real-time
LogPolicy: threaded

# KukaFRI specific configuration
KukaFRI:
  iiwa14: # Name of the robot in the controller
    host: 172.16.0.2 # IP of the robot
  iiwa7: # Name of an extra robot in the controller
    host: host.domain.com
    port: 30200 # This is the default and can be ommited
  # Actuated robots that are not controlled via mc_kuka_fri
  ignored: [env/door, env/box]
```

Run the program:

```bash
MCKukaFRI
```

You can also provide an additional configuration file (to swap between different network configurations easily for example):

```bash
MCKukaFRI -f conf.yaml
```

[![I.AM.Logo](https://i-am-project.eu/templates/yootheme/cache/iam_logo-horizontaal_XL-9e4a8a2a.png)](https://i-am-project.eu/index.php)

This work was partially supported by the Research Project I.AM. through the European Union H2020 program under GA 871899.

[iiwa_description]: https://github.com/epfl-lasa/iiwa_ros/tree/master/iiwa_description
[kuka_fri]: https://github.com/epfl-lasa/kuka_fri
[mc_rtc]: https://github.com/jrl-umi3218/mc_rtc
[mesh_sampling]: https://github.com/arntanguy/mesh_sampling
