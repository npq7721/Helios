Helios Core
=============

Setup
---------------------
Helios Core is the original Helios client and it builds the backbone of the network. It downloads and, by default, stores the entire history of Helios transactions (which is currently more than 100 GBs); depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to a day or more.

To download Helios Core, visit [helioscore.org](https://helioscoin.org/en/releases/).

Running
---------------------
The following are some helpful notes on how to run Helios on your native platform.

### Unix

Unpack the files into a directory and run:

- `bin/helios-qt` (GUI) or
- `bin/heliosd` (headless)

### Windows

Unpack the files into a directory, and then run helios-qt.exe.

### OS X

Drag Helios-Core to your applications folder, and then run Helios-Core.

### Need Help?

* See the documentation at the [Helios Wiki](https://en.helios.it/wiki/Main_Page)
for help and more information.
* Ask for help on [#helios](http://webchat.freenode.net?channels=helios) on Freenode. If you don't have an IRC client use [webchat here](http://webchat.freenode.net?channels=helios).
* Ask for help on the [HeliosTalk](https://heliostalk.org/) forums, in the [Technical Support board](https://heliostalk.org/index.php?board=4.0).

Building
---------------------
The following are developer notes on how to build Helios on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [Dependencies](dependencies.md)
- [OS X Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Windows Build Notes](build-windows.md)
- [OpenBSD Build Notes](build-openbsd.md)
- [Gitian Building Guide](gitian-building.md)

Development
---------------------
The Helios repo's [root README](/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Release Notes](release-notes.md)
- [Release Process](release-process.md)
- [Source Code Documentation (External Link)](https://dev.visucore.com/helios/doxygen/)
- [Translation Process](translation_process.md)
- [Translation Strings Policy](translation_strings_policy.md)
- [Travis CI](travis-ci.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [Shared Libraries](shared-libraries.md)
- [BIPS](bips.md)
- [Dnsseed Policy](dnsseed-policy.md)
- [Benchmarking](benchmarking.md)

### Resources
* Discuss on the [HeliosTalk](https://heliostalk.org/) forums, in the [Development & Technical Discussion board](https://heliostalk.org/index.php?board=6.0).
* Discuss project-specific development on #helios-core-dev on Freenode. If you don't have an IRC client use [webchat here](http://webchat.freenode.net/?channels=helios-core-dev).
* Discuss general Helios development on #helios-dev on Freenode. If you don't have an IRC client use [webchat here](http://webchat.freenode.net/?channels=helios-dev).

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [Files](files.md)
- [Fuzz-testing](fuzzing.md)
- [Reduce Traffic](reduce-traffic.md)
- [Tor Support](tor.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)
- [ZMQ](zmq.md)

License
---------------------
Distributed under the [MIT software license](/COPYING).
This product includes software developed by the OpenSSL Project for use in the [OpenSSL Toolkit](https://www.openssl.org/). This product includes
cryptographic software written by Eric Young ([eay@cryptsoft.com](mailto:eay@cryptsoft.com)), and UPnP software written by Thomas Bernard.
