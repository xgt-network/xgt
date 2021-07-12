## TODO

* Disable-able debug logging
* Variants on stack

```sh
sudo apt-get install -y build-essential cmake jq libboost-all-dev
```

```sh
brew install cmake jq
```

```sh
rm -rf build                       # removes build artifacts
mkdir -p build                     # create the build directory
(cd build && cmake ..)             # configure the project
(cd build && cmake --build .)      # build the project
(cd build && ./xgtvm -e'60 01 00'  # runs the project
```

To manage the project more easily, if you have Ruby installed:

```sh
rake -T         # list tasks that can be run
rake clean      # removes build artifacts
rake configure  # configures the project
rake make       # builds the project
rake test       # runs the project
rake            # configures and makes the project
```

Example finished machine data:

```json
{
  "returnValue": "",
  "finalState": {
  "pc": 5,
    "stack": ["8"]
  }
}
```
