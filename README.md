Welcome to the Embedded Controller firmware project.

The project is provided under the Apache 2.0 license (as found in the LICENSE file in the project’s GitHub repo).

Intel EC FW framework reference code is intended for prototyping and evaluation.

Visit documentation for getting started guide https://intel.github.io/ecfw-zephyr/index.html

# Building

If you never build before, first follow the instructions in the getting started guide mentioned above

Before building make sure you make the enviroment ready for building:
```shell
. ../ecfwwork/zephyr_fork/zephyr-env.sh
```
In order to build for Prodrive Atlas Board just do:
```shell
west build -c -p auto -b mec1501_adl_p_prodrive
```
