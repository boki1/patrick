## Patrick - library for using linear codes

[![dev-status](https://img.shields.io/badge/dev_status-in_progress-green)](https://img.shields.io/badge/dev_status-in_progress-green)
[![build-and-tests](https://github.com/boki1/patrick/actions/workflows/ci.yml/badge.svg)](https://github.com/boki1/patrick/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

---------------

**Building**

```sh
// Select what you would like to build when ccmake opens.
// Tests are automatically ran if built.
$ ./build.sh --output=cmake-build-debug --configure=yes

// Try the live demo
$ ./cmake-build-debug/app/livedemo

// Produce documentation and open it in the browser
$ doxygen docs/Doxyfile
$ xdg-open docs/build/html/index.html
```

**Documentation** - [_Latest_](https://boki1.github.io/patrick)

---------------

**LICENSE, MIT**
Kristiyan Stoimenov, 2023
