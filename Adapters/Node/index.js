'use strict';

// The optional CMake target emits `sc2opt.node`. Consumers may copy/package the module beside
// this loader or require the native module from their own package layout.
module.exports = require('./sc2opt.node');
