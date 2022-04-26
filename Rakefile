require 'fileutils'
require 'tmpdir'
require 'uri'
require 'json'
require 'bigdecimal'
require 'shellwords'
require 'rake/testtask'
import 'tasks/contracts.rake'
import 'tasks/lazy_wallets.rake'
autoload :Xgt, 'xgt/ruby'
autoload :Etc, 'etc'

case RUBY_PLATFORM
when /darwin/
  unless ENV['NO_HOMEBREW']
    `brew` rescue raise "This build requires homebrew presently. Set $NO_HOMEBREW and $CMAKE_PREFIX_PATH to manually bypass."
  end
  cmake_prefix_paths = (ENV["CMAKE_PREFIX_PATH"] || "").split(":")
  ENV['CMAKE_PREFIX_PATH'] = (cmake_prefix_paths + ["/usr/local/opt/openssl", "/usr/local/opt/icu4c"]).join(":")
end

directory "../xgt-build"

def from_genesis?
  ENV['FROM_GENESIS']&.upcase == 'TRUE'
end

def mining_disabled?
  ENV['MINING_DISABLED']&.upcase == 'FALSE'
end

def mining_threads
  # 0 set in the config will make xgtd use max logical CPUs
  ENV['MINING_THREADS'] || 0
end

def thread_count
  (ENV['THREAD_COUNT'] || Etc.nprocessors).to_i
end

def flush_chainstate?
  if ENV['FLUSH_TESTNET']
    STDERR.puts("FLUSH_TESTNET is deprecated, please use FLUSH_CHAINSTATE instead.")
  end
  ENV['FLUSH_CHAINSTATE']&.upcase == 'TRUE' || 
    ENV['FLUSH_TESTNET']&.upcase == 'TRUE'
end

def mining_error(message)
  return if mining_disabled?
  STDERR.puts("ABORT: #{message}")
  exit(2)
end

def wallet
  ENV['XGT_WALLET'] || mining_error("Wallet not specificed, please specify a wallet with 'XGT_WALLET'")
end

def wif
  ENV['XGT_WIF'] || mining_error("XGT_WIF not specified")
end

def recovery_private_key
  ENV['XGT_RECOVERY_PRIVATE_KEY'] || mining_error("XGT_RECOVERY_PRIVATE_KEY not specified")
end

def witness_private_key
  ENV['XGT_WITNESS_PRIVATE_KEY'] || mining_error("XGT_WITNESS_PRIVATE_KEY not specified")
end

def host
  ENV['XGT_HOST'] || 'http://localhost:8751'
end

def seed_hosts
  Array((ENV['XGT_SEED_HOST'] || "").split(","))
end

def instance_index
  ENV['XGT_INSTANCE_INDEX'].to_i
end

def port_offset
  ENV['XGT_PORT_OFFSET'].to_i
end

def config
  rpc.call('database_api.get_config', {})
end

def chain_id
  config['XGT_CHAIN_ID']
end

def address_prefix
  config['XGT_ADDRESS_PREFIX'] || 'XGT'
end

def fee
  "#{'%0.3f' % 0.0} XGT"
end

def unindent(str)
  lines = str.split("\n")
  lines.shift if lines.first.empty?
  prefix = (lines.first.match(/^(\s+)/) || [])[1] || ''
  lines.map { |l| l.gsub(/^#{prefix}/, '') }.join("\n")
end

def rpc
  Xgt::Ruby::Rpc.new(host)
end

desc 'Removes build artifacts'
task :clean do
  rm_rf "../xgt-build"
  rm_rf "../xgt-tests-build"
end

desc 'List available targets'
task :targets => "../xgt-build" do
  sh %( cmake --build .  --target help )
end

desc 'Runs CMake to prepare the project'
task :configure => "../xgt-build" do
  sh %( cmake -G Ninja -B ../xgt-build -S . -D CMAKE_BUILD_TYPE=debug )
end

desc 'Runs CMake to prepare the project for testnet'
task :configure_testnet => "../xgt-build" do
  sh %( cmake -G Ninja -B ../xgt-build -S . -D CMAKE_BUILD_TYPE=debug -D BUILD_XGT_TESTNET=ON )
end

task :test do
  sh %(
    cmake
      -D BUILD_XGT_TESTNET=ON
      -D BUILD_TESTING=TRUE
      -D COLOR_DIAGNOSTICS=ON
      -D CMAKE_BUILD_TYPE=Debug
      -G Ninja
      -B ../xgt-tests-build
      -S .
  ) 
  sh %( ninja -C ../xgt-tests-build chain_test )
end

desc 'Builds the project'
task :make do
  sh %( ninja -C ../xgt-build xgtd -j#{thread_count})
end

desc 'Build all targets'
task :make_all => :configure do

  tlibs = %w(
    rocksdb
    rocksdb-shared
    core_tools
    ldb
    project_secp256k1
    bip_lock
    rebuild_cache
    edit_cache
    fc
    sst_dump
    xgt_schema
    db_fixture
    dump_xgt_schema
    get_dev_key
    sign_digest
    sign_transaction
    js_operation_serializer
    size_checker
    xgtd
    cat-parts
    graphene_net
    mira

    appbase
    appbase_example
    chainbase

    hash_table_bench
    range_del_aggregator_bench
    db_bench
    cache_bench
    filter_bench
    memtablerep_bench
    table_reader_bench

    webserver_plugin
    witness_plugin
    wallet_by_key_plugin
    wallet_history_plugin
    wallet_history_rocksdb_plugin
    block_api_plugin
    chain_api_plugin
    contract_api_plugin
    database_api_plugin
    test_api_plugin
    transaction_api_plugin
    wallet_by_key_api_plugin
    wallet_history_api_plugin
    chain_plugin
    debug_node_plugin
    json_rpc_plugin
    p2p_plugin
    transaction_status_plugin

    gtest
    test_sqrt
    testharness
    test_fixed_string
    test_shared_mem
    ecc_test
    log_test
    ecdsa_canon_test
    saturation_test
    chainbase_test
    schema_test
    chain_test
    test_block_log

    xgt_plugins
    xgt_utilities
    xgt_protocol
    xgt_chain
    xgtd
  )

  borked = %w(
    bloom_test
    plugin_test
    inflation_model
    serialize_set_properties
    real128_test
    all_tests
    blind
    hmac_test
    thread_test
    task_cancel_test
  )

  sh %( ninja -C ../xgt-build #{tlibs.join(" ")} )
end

task :bin_tests do
  xrun = Proc.new() { |command| sh "cd ../xgt-build && ./programs/util/#{command}" }
  xrun.call "schema_test"
  xrun.call "test_block_log"
  xrun.call "test_fixed_string"
  xrun.call "test_shared_mem"
  xrun.call "test_sqrt"
end

desc 'Strip the binary of unneeded symbols'
task :strip do
  sh %(cd ../xgt-build && strip --strip-unneeded ./programs/xgtd/xgtd)
end

task :build_release => [:clean, :configure, :make, :strip]

desc 'Runs a basic example instance locally'
task :run do
  data_dir = "../xgt-chainstate-#{instance_index}"

  if flush_chainstate?
    sh "rm -rf #{data_dir}"
  end
  sh "mkdir -p #{data_dir}"

  # TODO: Needs revisiting
  their_host = if host
    uri = URI.parse(host)
    %(#{uri.host}:#{uri.port})
  else
    nil
  end

  my_host = '0.0.0.0'
  File.open(File.join(data_dir, 'config.ini'), 'w') do |f|
    f.puts(unindent(%(
      log-console-appender = {"appender":"stderr","stream":"std_error"}
      log-file-appender = {"appender":"logfile","file":"logfile.log"}
      log-logger = {"name":"default","level":"debug","appender":"stderr"}
      log-logger = {"name":"default","level":"debug","appender":"logfile"}
      #log-logger = {"name":"sync","level":"debug","appender":"stderr"}
      #log-logger = {"name":"sync","level":"debug","appender":"logfile"}
      log-logger = {"name":"p2p","level":"info","appender":"stderr"}
      log-logger = {"name":"p2p","level":"info","appender":"logfile"}

      backtrace = yes

      shared-file-size = 12G

      p2p-endpoint = #{my_host}:#{2001 + port_offset + instance_index}
      webserver-http-endpoint = #{my_host}:#{8751 + port_offset + instance_index * 2}

      miner = ["#{wallet}","#{wif}"]
      mining-threads = #{mining_threads}
      witness = "#{wallet}"
      private-key = #{recovery_private_key}
      mining-reward-key = #{witness_private_key}

      enable-stale-production = #{mining_disabled? ? 'false' : 'true'}
    )))
    if from_genesis?
      f.puts "p2p-seed-node = "
    elsif seed_hosts && seed_hosts.any?
      f.puts "p2p-seed-node = #{seed_hosts.join(" ")}"
    end
    f.puts "p2p-seed-node = "
  end
  $stderr.puts(File.read("#{data_dir}/config.ini"))

  flags = ['--data-dir=.']
  if from_genesis?
    flags << '--from-genesis'
  end
  sh %(cd #{data_dir} && ../xgt-build/programs/xgtd/xgtd #{flags.join(' ')})
end

desc 'Get approximate C++ LoC'
task :wc do
  sh %(wc -l #{Dir.glob('**/*.{c,h}pp').join(' ')})
end

def generate_keys
  master = Xgt::Ruby::Auth.random_wif
  ks = { 'master' => master }
  %w(recovery money social memo).each do |role|
    private_key = Xgt::Ruby::Auth.generate_wif(wallet, master, 'recovery')
    public_key = Xgt::Ruby::Auth.wif_to_public_key(private_key, address_prefix)
    ks["#{role}_private"] = private_key
    ks["#{role}_public"] = public_key
  end

  response = rpc.call('wallet_by_key_api.generate_wallet_name', {
    'recovery_keys' => [ks['recovery_public']]
  })
  wallet_name = response['wallet_name']
  ks['wallet_name'] = wallet_name

  ks
end

def create_wallet!(keys)
  name = keys['wallet_name']

  txn = {
    'extensions' => [],
    'operations' => [
      {
        'type' => 'wallet_create_operation',
        'value' => {
          'fee' => {
            'amount' => '0',
            'precision' =>  8,
            'nai' => '@@000000021'
          },
          'creator' => wallet,
          'recovery' => {
            'weight_threshold' => 1,
            'account_auths' => [],
            'key_auths' => [[keys['recovery_public'], 1]]
          },
          'money' => {
            'weight_threshold' => 1,
            'account_auths' => [],
            'key_auths' => [[keys['money_public'], 1]]
          },
          'social' => {
            'weight_threshold' => 1,
            'account_auths' => [],
            'key_auths' => [[keys['social_public'], 1]]
          },
          'memo_key' => keys['memo_public'],
          'json_metadata' => '',
          'extensions' => []
        }
      }
    ]
  }

  $stderr.puts(%(Creator is "#{wallet}" with wif "#{wif}"...))
  $stderr.puts(%(Creating wallet with master key "#{keys['master']}"...))
  signed = Xgt::Ruby::Auth.sign_transaction(rpc, txn, [wif], chain_id)
  rpc.call('transaction_api.broadcast_transaction', [signed])
  name
end

namespace :machine do
  src_dir = File.join(File.dirname(__FILE__), %(../xgtvm))
  dest_dir = File.join(File.dirname(__FILE__), %(libraries/vendor/xgtvm))
  dest_git_dir = File.join(dest_dir, '.git')

  desc 'Remove machine-related files'
  task :clean do
    FileUtils.rm_r(dest_dir) if Dir.exist?(dest_dir)
  end

  desc 'Vendor machine-related files'
  task :vendor => [:clean] do
    raise %(Directory #{src_dir} doesn't exist!) unless Dir.exist?(src_dir)
    FileUtils.cp_r(src_dir, dest_dir)
    FileUtils.rm_r(dest_git_dir) if File.exists?(dest_git_dir)
  end
end
