![XGT Logo](https://xgt-network.github.io/assets/images/xgt-logo.png)

# Sound Money & Robust Programmability



Infinite scalablilty for financial inclusion With native smart contracts and robust architecture, XGT supports up to 3,000 transactions per second and a financially-inclusive experience for all users.

Dockerized libraries for egalitarian opportunity to run full nodes An XGT daemon can be easily run with the provided Docker images, which enables exchanges, developers and applications to integrate and run XGT with unparalled efficiency, while also embracing reliability as a key security measure. Deploying and maintaining an XGT node is simple enough that anyone can do it.

Optimal governance for egalitarian participation XGT will be supported by an unlimited number of nodes run by developers, applications, users and miners. Miners produce the blocks that track all XGT transactions at a rate of one block every three seconds and there is no limit on the number of miners. Together these nodes provide growing decentralization and security to XGT.

Streamlined full nodes for perfect app integration The advantage of XGT nodes is that they are extremely quick to set up, inexpensive to operate or migrate to, and highly responsive relative to other networks. Better still, the open nature of the network means that it is available to all, and participants are not subject to the behaviour of a group of insiders. Anyone can operate a full node, or mine using the open source software. XGT is built with the knowledge that the benefit of a full node designed specifically for economic purposes, knowing full well non-economic data needs to be supported by off-chain, application-based APIs for optimal performance.



## Documents

Documentation can be found at the [XGT Documentation Hub](https://xgt-network.github.io/).

### Usage
<ul class="ul-buttons">
  <li><a href="https://xgt-network.github.io/compilation/">Compiling XGT</a></li>
  <li><a href="https://xgt-network.github.io/mining/">Mining</a></li>
  <li><a href="https://xgt-network.github.io/ubuntu_quickstart.html">Ubuntu Quickstart</a></li>
</ul>

### API Docs
<ul class="ul-buttons">
  <li><a href="https://xgt-network.github.io/json_rpc.html">JSON RPC Documentation</a></li>
  <li><a href="https://xgt-network.github.io/schema_outline.html">XGT Schema</a></li>
</ul>

### Libraries
<ul class="https://xgt-network.github.ioul-buttons">
  <li><a href="/xgt_ruby_recipes.html">xgt-ruby recipes</a></li>
</ul>


## Advantages

* Fast application and exchange integration: New streamlined blockchain infrastructure based on years of research.
* Real-time financial inclusion: Three-second block times.
* Decentralization: Proof of Work for network security, coin supply, and open governance.
* Revolutionary application development: Off-chain content and on-chain reference-IDs for rewarding community engagement
* Unlimited financial inclusion:
* On-chain currency creation with vote-able emissions for community growth via XGT Token Trees
* Unparalled bridging of applications and blockchain: base58 randomized wallet IDs for privacy and integration with off-chain account systems. Generated with a BTC compliant algorithm, and namespaced with XGT for user-safety

## Technical Details

* Currency symbol XGT
* Energy System, used to power SmartContracts without coin burn
* Proof of Work consensus
* ~18,700,000 XGT for egalitarian distribution, 21,000,000 token supply schedule.
* Mining halvings, targetted for sync with BTC supply halvings

# Installation

Getting started with XGT is fairly simple. You can either choose to use docker-images, build with docker manually or build from source directly. All steps have been documented and while many different OS are supported, the easiest one is Ubuntu 16.04.

## Quickstart

Just want to get up and running quickly? We have pre-built Docker images for your convenience. More details are in our [Mining XGT](https://github.com/xgt-network/xgt/wiki/Mining-XGT) Guide.

## Building

We **strongly** recommend using one of our pre-built Docker images or using Docker to build XGT. Both of these processes are described in the [Mining XGT](https://github.com/xgt-network/xgt/wiki/Mining-XGT).

To build from source, we also have [build instructions](https://github.com/xgt-network/xgt/wiki/Compiling-XGT) for Linux (Ubuntu 20.04 LTS) and macOS.

## Dockerized P2P Node

To run a p2p node (ca. 2GB of memory is required at the moment):

    docker run \
        -d -p 2001:2001 -p 8090:8090 --name xgtd-default \
        xgt-network/xgt

    docker logs -f xgtd-default  # follow along

## Dockerized Full Node

See [Mining XGT](https://github.com/xgt-network/xgt/wiki/Mining-XGT)

## CLI Wallet

We provide a basic cli wallet for interfacing with `xgtd`. The wallet is self-documented via command line help. See [xgt-ruby](https://github.com/xgt-network/xgt-ruby)

## Testing

See [doc/devs/testing.md](doc/devs/testing.md) for test build targets and info
on how to use lcov to check code test coverage.

# Configuration

## Config File

Run `xgtd` once to generate a data directory and config file. The default location is `witness_node_data_dir`. Kill `xgtd`. It won't do anything without seed nodes. If you want to modify the config to your liking, we have two example configs used in the docker images. ( [consensus node](contrib/config-for-docker.ini), [full node](contrib/fullnode.config.ini) ) All options will be present in the default config file and there may be more options needing to be changed from the docker configs (some of the options actually used in images are configured via command line).

## Seed Nodes

A list of some seed nodes to get you started can be found in
[doc/seednodes.txt](doc/seednodes.txt).

This same file is baked into the docker images and can be overridden by
setting `XGT_SEED_NODES` in the container environment at `docker run`
time to a whitespace delimited list of seed nodes (with port).

## Environment variables

There are quite a few environment variables that can be set to run xgtd in different ways:

* `USE_WAY_TOO_MUCH_RAM` - if set to true, xgtd starts a 'full node'
* `USE_FULL_WEB_NODE` - if set to true, a default config file will be used that enables a full set of API's and associated plugins.
* `USE_NGINX_FRONTEND` - if set to true, this will enable an NGINX reverse proxy in front of xgtd that proxies WebSocket requests to xgt. This will also enable a custom healthcheck at the path '/health' that lists how many seconds away from current blockchain time your node is. It will return a '200' if it's less than 60 seconds away from being synced.
* `USE_MULTICORE_READONLY` - if set to true, this will enable xgtd in multiple reader mode to take advantage of multiple cores (if available). Read requests are handled by the read-only nodes and write requests are forwarded back to the single 'writer' node automatically. NGINX load balances all requests to the reader nodes, 4 per available core. This setting is still considered experimental and may have trouble with some API calls until further development is completed.
* `HOME` - set this to the path where you want xgtd to store it's data files (block log, shared memory, config file, etc). By default `/var/lib/xgt` is used and exists inside the docker container. If you want to use a different mount point (like a ramdisk, or a different drive) then you may want to set this variable to map the volume to your docker container.

## System Requirements

4GB memory (16GB recommended)
x86 or AMD64 compatible CPU

# No Support & No Warranty

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
