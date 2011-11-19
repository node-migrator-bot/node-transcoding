#!/usr/bin/env node

var fs = require('fs');
var path = require('path');
var transcode = require('transcode');
var util = require('util');

var opts = require('tav').set({
});

if (opts.args.length < 1) {
  console.log('no input file specified');
  return;
}

var inputFile = path.normalize(opts.args[0]);
if (!path.existsSync(inputFile)) {
  console.log('input file not found');
  return;
}

transcode.queryInfo(inputFile, function(err, info) {
  util.puts('Info for ' + inputFile + ':');
  util.puts(util.inspect(info, false, 3));
});
