# SEMUD communication protocol
This repository contains an implementation of **SEMUD**'s communication protocol for the [Click Modular Router](http://www.read.cs.ucla.edu/click/).
The end-to-end communication protocol runs on layer 2, is based on *Castor* (hence the original project name), and features advanced security properties such as resilience against advanced attacks, such as blackhole, greyhole, wormhole, and replay attacks.

**Discalimer:** *This is highly experimental code and is expected to break. Feel free to [contact me](mailto:mschmittner@seemoo.tu-darmstadt.de) if you encounter problems.*

## Table of Contents
* [Code Naviation](#code-navigation)
* [Install](#install)
    * [General](#general)
    * [Cross Compilation](#cross-compilation)
    * [Android](#android)
* [Run](#run)
    * [Interaction at Runtime](#interaction-at-runtime)
    * [Tools](#tools)
* [Contributors](#contributors)
* [License](#license)
* [Related Publications](#related-publications)

## Code Navigation
This section gives a rough overview where relevant code is located.
* `conf/castor/`: Click script files.
    * `castor_settings.click`: Castor-specific parameters etc.
    * `castor_run_PROFILE.click`: Run configurations where `PROFILE` can be `userlevel`, `ns3`, or `benchmark`.
    * other `.click`: shared modules which are included by the run configurations.
* `elements/castor/`: C++ source code.
    * `castor.hh`: packet definitions
    * `attack/`: elements specific to implement certain attacks
    * `crypto/`: all required crypto (includes a wrapper class around `libsodium`)
    * `flow/`: flow generation and reconstruction
    * `neighbors/`: neighbor discovery and neighbor-to-neighbor authentication
    * `ratelimiter/`: prevents neighbors from flooding the network
    * `routing/`: main code base: routing logic
    * `util/`: utility Elements
* `apps/castor/`: applications that allow interaction with specific `click-castor` elements.

## Install

This repository comes with convenience scripts to build the protocol with its only dependency **[libsodium](https://download.libsodium.org/doc/)**.

### General
`click-castor` can be built as a regular userlevel or [ns-3](https://www.nsnam.org/) module.
After cloning this repository, simply run
```bash
# Userlevel or ...
./build-castor-native.sh
# ... ns-3
./build-castor-ns3.sh
```

If you plan to *extend the code* (e.g., altered or new `Elements` in the `elements/` direcotry), you only need to run 
```bash
make elemlist all
```

### Cross Compilation
To build Click for a different architecture (such as i386 in our mesh nodes) on a x64 machine, you can cross compile by adding `--host=i386-linux-gnu` to the `./configure` call in `build-castor-native.sh`. Make sure to include the proper header files, e.g., `chroot` in the target file system.

### Android
The Android build scripts use standalone toolchains. This requires the `NDK_ROOT` environment variable to point to your [Android NDK](https://developer.android.com/ndk/) installation (tested with version **14b**).
Depending on `ARCH` (currently `armv7-a` or `armv8-a`), simply run
```
./build-android-ARCH.sh
```
The relevant `click` binary is then found in `android/build/click-android-ARCH/`.
The libsodium shared library will be in `libsodium/libsodium-android-ARCH/`.


## Run
Castor uses a single network interface for communication with other nodes. By default, this interface is `wlan0`. This can be changed either in the `conf/castor/castor_io_userlevel.click` configuration file (look for variable `EthDev`) or by specifying `EthDev=wlanX` when starting Click.

```bash
# Run Castor ...
userlevel/click conf/castor/castor_run_userlevel.click
# ... or with non-standard network interface:
userlevel/click EthDev=wlanX conf/castor/castor_run_userlevel.click
```

Alternatively, you can run a benchmark configuration which does not require any network interfaces:
```bash
userlevel/click conf/castor/castor_run_benchmark.click
```

### Interaction at Runtime
Click's `Element` handlers can be accessed using a [ControlSocket Element](http://read.cs.ucla.edu/click/elements/controlsocket).
By default, Castor starts with two sockets opened (see `conf/castor/castor_socket.click).
- Unix socket under `/tmp/click-castor.sock`, and
- TCP socket at port 7777.

After connecting with the socket, one can read data from the Element Handlers using a line-based protocol described [here](http://read.cs.ucla.edu/click/elements/controlsocket).
For example, one could read the list of neighbouring nodes by sending the command `READ neighbors.print` to the socket, which would lead to an answer of `200 OK\r\nDATA N\r\nx_1x_2x_n` where `N` denotes the length of the returned data and `x_1` to `x_n` are the data symbols.

### Tools
Castor provides a number of tools for interaction (see `apps/castor/`). To build them, run
```bash
cd apps/castor
make
```
* **status** Queries the neighbor and routing tables.
* **ping** Perform a classic `ping` using Castor packets. *Debug packets do not tamper with current routing state*.
* **traceroute** Trace packets through the network. Can be used to find all currently available paths to the destination. *Debug packets do not tamper with current routing state*.

## Contributors
* **Milan Schmittner** ([email](mailto:mschmittner@seemoo.tu-darmstadt.de), [web](https://seemoo.de/mschmittner), *maintainer*)
* Daniel Steinmetzer (early version of the original *Castor* implementation)
* Michael Koch (port to *Android*)
* Simon Schmitt (`ping` and `traceroute` tools)

## License
* `click-castor` is released under the [GNU General Public License (GPL) version 3](http://www.gnu.org/licenses/).
* The Click framework has its own license (partly MIT and GPLv2), see the `README` file for details.

## Publications

### Main paper (please cite if you use this code)
* M. Schmittner, A. Asadi, and M. Hollick, “**SEMUD: Secure Multi-hop Device-to-Device Communication for 5G Public Safety Networks**,” in *IFIP Networking Conference (NETWORKING)*, June 2017. ([PDF](http://dl.ifip.org/db/conf/networking/networking2017/1570334533.pdf))

### Further related publication
* M. Schmittner and M. Hollick, “**Xcastor: Secure and Scalable Group Communication in Ad Hoc Networks**,” in *IEEE Symposium on a World of Wireless, Mobile and Multimedia Networks (WoWMoM)*, June 2016. ([PDF](https://www.informatik.tu-darmstadt.de/fileadmin/user_upload/Group_SEEMOO/milan_schmittner/xcastor-wowmom16.pdf))
* M. Schmittner, “**Secure and Scalable Multicast in Mobile Ad-hoc Networks**,” *Master thesis*, Technische Universität Darmstadt, September 2014. ([PDF](http://tuprints.ulb.tu-darmstadt.de/6607/))
* W. Galuba, P. Papadimitratos, M. Poturalski, K. Aberer, Z. Despotovic, and W. Kellerer, “**Castor: Scalable Secure Routing for Ad Hoc Networks**,” in *Proceedings of the IEEE Conference on Computer Communications (INFOCOM)*, March 2010. ([PDF](https://infoscience.epfl.ch/record/148217/files/castor.pdf))
* E. Kohler, R. Morris, B. Chen, J. Jannotti, and M. F. Kaashoek, “**The Click Modular Router**,” *ACM Transactions on Computer Systems*, vol. 18, no. 3, pp. 263–297, August 2000. ([PDF](https://pdos.csail.mit.edu/papers/click:tocs00/paper.pdf), [web](http://read.cs.ucla.edu/click/click))
