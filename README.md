<!-- <a target="_blank" href="http://semver.org">![Version][badge.version]</a> -->
<!-- <a target="_blank" href="https://cirrus-ci.com/github/k-nuth/rpc">![Build Status][badge.Cirrus]</a> -->

# RPC (deprecated) <a target="_blank" href="https://github.com/k-nuth/rpc/releases">![Github Releases][badge.release]</a> <a target="_blank" href="https://travis-ci.org/k-nuth/rpc">![Build status][badge.Travis]</a> <a target="_blank" href="https://ci.appveyor.com/projects/k-nuth/rpc">![Build Status][badge.Appveyor]</a> <a href="#">![CPP][badge.cpp]</a> <a target="_blank" href="https://t.me/knuth_cash">![Telegram][badge.telegram]</a> <a target="_blank" href="https://k-nuth.slack.com/">![Slack][badge.slack]</a>

# *Warning*: This module is obsolete and out of maintenance.

> JSON-RPC library

## Getting started

Installing the library is as simple as:

1. Install and configure the Knuth build helper:
```
$ pip install kthbuild --user --upgrade

$ conan remote add kth https://api.bintray.com/conan/k-nuth/kth
```

2. Install the appropriate library:

```
$ conan install rpc/0.X@kth/stable 
```

For more more detailed instructions, please refer to our [documentation](https://kth.cash/docs/).

## About this library

This library can be used stand-alone, but it is probably convenient for you to use one of our main projects, [look over here](https://github.com/k-nuth/kth/).

## Issues

Each of our modules has its own Github repository, but in case you want to create an issue, please do so in our [main repository](https://github.com/k-nuth/kth/issues).



<!-- Links -->
[badge.Travis]: https://travis-ci.org/k-nuth/rpc.svg?branch=master
[badge.Appveyor]: https://ci.appveyor.com/api/projects/status/github/k-nuth/rpc?svg=true&branch=master
[badge.Cirrus]: https://api.cirrus-ci.com/github/k-nuth/rpc.svg?branch=master
[badge.version]: https://badge.fury.io/gh/k-nuth%2Frpc.svg
[badge.release]: https://img.shields.io/github/release/k-nuth/rpc.svg
[badge.cpp]: https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B
[badge.telegram]: https://img.shields.io/badge/telegram-badge-blue.svg?logo=telegram
[badge.slack]: https://img.shields.io/badge/slack-badge-orange.svg?logo=slack

<!-- [badge.Gitter]: https://img.shields.io/badge/gitter-join%20chat-blue.svg -->
