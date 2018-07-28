# libprotobuf-mutator for Apollo

## Overview
The repository is forked from [google/libprotobuf-mutator](https://github.com/google/libprotobuf-mutator)

libprotobuf-mutator is a library to randomly mutate [protobuffers](https://github.com/google/protobuf) messages. 

It could be used together with guided fuzzing engine: [libFuzzer](http://libfuzzer.info). 

Customized for fuzzing test against Baidu [Apollo](https://github.com/ApolloAuto/apollo) self-driving functionalities
## Prerequisite

To use libfuzzer, install clang-6.0 in the Apollo docker. 

```
sudo apt-add-repository "deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-6.0 main"
wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo apt-get update
sudo apt-get install clang-6.0 lldb-6.0 lld-6.0
```

After merging the pull request, you should have the following **requirements** ready
* `CROSSTOOL` file for clang-6.0 at `/apollo/tools/clang-6.0/`
* Several fuzz driver derived from unit test under `/apollo/modules/controller/` and `/apollo/modules/integration_test/` with the name contains `_fuzz`
* Updated `WORKSPACE.in` by adding a new git repository for a modified version of libprotbuf-mutator
* `BUILD` file for the third-party library located in `/apollo/third_party/libprotobuf-mutator.BUILD`

Before moving on to build the target module with fuzzing support, we need to fix one minor issue with `libfortran` in the Apollo docker
```
sudo ln -s /usr/lib/x86_64-linux-gnu/libgfortran.so.3 /usr/lib/libgfortran.so
```
## Usage
To compile the fuzzing unit test, using bazel build with the provided CROSSTOOL. We will use one of the controller fuzzing unit test as an example
```
bazel build --crosstool_top=tools/clang-6.0:toolchain modules/control/controller:lat_controller_fuzzer --compilation_mode=dbg
```
The target fuzzing unit test binary will be built in `bazel-bin` folder. 
If you take a look at the corresponding `BUILD` file:
```
cc_binary(
    name = "lat_controller_fuzzer",
    srcs = ["lat_controller_fuzzer.cc"],
    data = ["//modules/control:control_testdata"],
    deps = [
        ":lat_controller",
        "//modules/common:log",
        "//modules/common/time",
        "//modules/common/util",
        "//modules/common/vehicle_state:vehicle_state_provider",
        "//modules/control/proto:control_proto",
        "//modules/planning/proto:planning_proto",
        "@libprotobuf_mutator//:mutator",
    ],
    copts = ["-fsanitize=fuzzer,address",
    "-Iexternal/libprotobuf_mutator/src/",],
    linkopts = ["-fsanitize=fuzzer,address"]
)
```
The binary is compiled with the option `-fsanitize=fuzzer,address`, the `fuzzer` option tells compiler to use `libFuzzer` driver to build, while the `address` adds AddressSanitizer. You can optionally add 
* `UndefinedBehaviorSanitizer`
* `LeakSanitizer`
* `ThreadSanitizer`
* etc. 

To Disable GLOG output, which is unnecessary for the fuzzing, set the environment variable
```
GLOG_minloglevel=4
```

Now, if we run the binary, the fuzzing test will start, and some runtime statistics will appear on the screen. If a crash is encountered, the fuzzing will stop, and the testcase will be saved. For more advanced commandline options, such as using seeds and dictionaries, please refer to the [libFuzzer](http://libfuzzer.info) page. 

The `ProtobufMutator` class implements mutations of the protobuf **tree structure** and mutations of **individual fields**. The field mutation logic is preliminary -- it can be further improved by adding more domain specific knowledge (e.g., generating valid sequence number for repeated fields), and we are working on it. 

`gdb` is prefered to be used for root cause diagnosis, and some simple instructions are provided
```
gdb bazel-bin/control/controller/lon_controller_fuzzer
set args ./crash-0x1234567
run 
bt  # output backtrace of crash
```

See also the `modules/control/controller/lat_controller_fuzzer.cc` and `modules/control/controller/simple_control_fuzz.cc` in the pull request for more examples.

Here is a prototype of the fuzzer driver to be written for developers. 

```
#include "src/libfuzzer/libfuzzer_macro.h"

DEFINE_PROTO_FUZZER(const MyMessageType& input) {
  // Code which needs to be fuzzed.
  ConsumeMyMessageType(input);
}
```
## Write Your Own Fuzz Test
The easist way to get start is to write the Fuzz testcase based on the existing unit tests. Following these steps to get start:
* Copy the `*_test.cc` into `*_fuzz.cc` under submodule folders
* Include `libfuzzer/libfuzzer_macro.h`
* Use each gtest `TEST_F` as target function by changing its interface (refering to examples)
* Implement `DEFINE_PROTO_FUZZER` accordingly to call the target functions
* Add the target into BUILD files with required dependency, compilation options, and link options added 
* Build & run

## Contact
Yunhan Jia [@jiayunhan](https://github.com/jiayunhan)

For Baidu developers, if you have any questions, you can reach out to me on **Baidu-Hi**.

## Acknowledgement
**libprotobuf-mutator for Apollo** is forked from the Google open source [project](https://github.com/google/libprotobuf-mutator), and is provided under Apache Version 2.0 license. 
