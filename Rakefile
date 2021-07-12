require 'fileutils'
require 'tmpdir'
require 'uri'
require 'json'
require 'bigdecimal'
require 'shellwords'
require 'rake/testtask'
autoload :Xgt, 'xgt/ruby'

def mining_disabled?
  ENV['MINING_DISABLED']&.upcase == 'TRUE'
end

def mining_threads
  # 0 set in the config will make xgtd use max logical CPUs
  ENV['MINING_THREADS'] || 0
end

def flush_testnet?
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
  sh 'rm -rf ../xgt-build'
end

desc 'Runs CMake to prepare the project'
task :configure do
  sh %(
    mkdir -p ../xgt-build \
      && cd ../xgt-build \
      && cmake -D CMAKE_BUILD_TYPE=RelWithDebInfo \
               --target xgtd \
               ../xgt
  )
end

desc 'Builds the project'
task :make do
  count = ENV['THREAD_COUNT'].to_i
  count = 2 if count == 0
  sh %(cd ../xgt-build && cmake --build . --target xgtd -- -j#{count})
end

desc 'Runs a basic example instance locally'
task :run do
  data_dir = "../xgt-build/chain-data-#{instance_index}"

  if flush_testnet?
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
      #log-logger = {"name":"p2p","level":"debug","appender":"stderr"}
      #log-logger = {"name":"p2p","level":"debug","appender":"logfile"}

      backtrace = yes

      shared-file-size = 12G

      p2p-endpoint = #{my_host}:#{2001 + instance_index}
      webserver-http-endpoint = #{my_host}:#{8751 + instance_index * 2}

      miner = ["#{wallet}","#{wif}"]
      mining-threads = #{mining_threads}
      witness = "#{wallet}"
      private-key = #{recovery_private_key}
      mining-reward-key = #{witness_private_key}

      enable-stale-production = #{mining_disabled? ? 'false' : 'true'}
    )))
    if seed_hosts && seed_hosts.any?
      f.puts "p2p-seed-node = #{seed_hosts.join(" ")}"
    end
  end
  $stderr.puts(File.read("#{data_dir}/config.ini"))

  sh %(cd #{data_dir} && ../programs/xgtd/xgtd --data-dir=.)
end

desc 'Builds the tests'
task :make_tests do
  count = ENV['THREAD_COUNT'].to_i
  count = 2 if count == 0
  # TODO: XXX: Gradually expand tests
  sh %(cd ../xgt-build && cmake --build . --target chain_test -- -j#{count})
end

desc 'Runs the tests'
task :run_tests do
  sh 'cd ../xgt-build && ./tests/chain_test'
  # TODO: Identify other tests
end

desc 'Get approximate C++ LoC'
task :wc do
  sh %(wc -l #{Dir.glob('**/*.{c,h}pp').join(' ')})
end

namespace :lazy_wallets do
  task :name_test do
    generate_keys = ->() {
      master = Xgt::Ruby::Auth.random_wif
      ks = { 'master' => master }
      %w(recovery money social memo).each do |role|
        private_key = Xgt::Ruby::Auth.generate_wif(wallet, master, 'recovery')
        public_key = Xgt::Ruby::Auth.wif_to_public_key(private_key, address_prefix)
        ks["#{role}_private"] = private_key
        ks["#{role}_public"] = public_key
      end
      ks
    }

    master = Xgt::Ruby::Auth.random_wif
    private_key = Xgt::Ruby::Auth.generate_wif(wallet, master, 'recovery')
    public_key = Xgt::Ruby::Auth.wif_to_public_key(private_key, address_prefix)

    response = rpc.call('wallet_by_key_api.generate_wallet_name', {
      'recovery_keys' => [public_key]
    })
    wallet_name = response['wallet_name']

    txn = {
      'extensions' => [],
      'operations' => [
         {
           'type' => 'transfer_operation',
           'value' => {
             'amount' => {
               'amount' => '1',
               'precision' =>  8,
               'nai' => '@@000000021'
             },
             'from' => 'XGT0000000000000000000000000000000000000000',
             'to' => wallet_name,
             'json_metadata' => '',
             'extensions' => []
           }
        }
      ]
    }

    id = rpc.broadcast_transaction(txn, [wif], chain_id)
    (puts 'Waiting...' or sleep 1) until rpc.transaction_ready?(id)

    keys = generate_keys.call
    txn = {
      'extensions' => [],
      'operations' => [
        {
          'type' => 'wallet_update_operation',
          'value' => {
            'wallet' => wallet_name,
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

    p txn
    id = rpc.broadcast_transaction(txn, [private_key], chain_id)
    (puts 'Waiting...' or sleep 1) until rpc.transaction_ready?(id)
  end
end

namespace :catalyst do
  generate_keys = ->() {
    FileUtils.mkdir_p('out')
    File.open('out/keys.json', 'w') do |f|
      master = Xgt::Ruby::Auth.random_wif
      ks = { 'master' => master }
      %w(recovery money social memo witness).each do |role|
        private_key = Xgt::Ruby::Auth.generate_wif(wallet, master, role)
        public_key = Xgt::Ruby::Auth.wif_to_public_key(private_key, address_prefix)
        ks["#{role}_private"] = private_key
        ks["#{role}_public"] = public_key
      end
      response = rpc.call('wallet_by_key_api.generate_wallet_name', {
        'recovery_keys' => [ks['recovery_public']]
      })
      ks['wallet_name'] = response['wallet_name']
      f.puts(JSON.pretty_generate(ks))
    end
  }

  keys = ->() {
    JSON.load(File.open('out/keys.json'))
  }

  does_witness_exist = ->() {
    name = wallet_name.call
    response = rpc.call('database_api.list_witnesses', { 'start' => name, 'limit' => 1, 'order' => 'by_name'}) || {}
    witnesses = response['witnesses'] || []
    witness = witnesses.first || {}
    (name == witness['owner'])
  }

  desc 'Generate the keys for the witness'
  task :generate_keys do
    generate_keys.call
  end

  desc 'Create a wallet for the witness'
  task :create_wallet do
    name = keys.call['wallet_name']

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
              'key_auths' => [[keys.call['recovery_public'], 1]]
            },
            'money' => {
              'weight_threshold' => 1,
              'account_auths' => [],
              'key_auths' => [[keys.call['money_public'], 1]]
            },
            'social' => {
              'weight_threshold' => 1,
              'account_auths' => [],
              'key_auths' => [[keys.call['social_public'], 1]]
            },
            'memo_key' => keys.call['memo_public'],
            'json_metadata' => '',
            'extensions' => []
          }
        }
      ]
    }

    $stderr.puts(%(Creator is "#{wallet}" with wif "#{wif}"...))
    $stderr.puts(%(Creating wallet with master key "#{keys.call['master']}"...))
    signed = Xgt::Ruby::Auth.sign_transaction(rpc, txn, [wif], chain_id)
    rpc.call('transaction_api.broadcast_transaction', [signed])
  end

  desc 'Register the witness'
  task :register do
    name = keys.call['wallet_name']

    components = fee.split(' ')
    decimal = BigDecimal(components.first) * 1
    final_fee = decimal.truncate.to_s + '.' + sprintf('%03d', (decimal.frac * 1000).truncate) + ' ' + components.last

    # raise 'Witness already registered!' if does_witness_exist.call

    txn = {
      'extensions' => [],
      'operations' => [{
        'type' => 'witness_update_operation',
        'value' => {
          'owner' => name,
          'url' => 'http://witness-category/my-witness',
          'block_signing_key' => keys.call['witness_public'],
          'props' => {
            'account_creation_fee' => {'amount'=>'0','precision'=>8,'nai'=>'@@000000021'}
          },
          'fee' => {'amount'=>'0','precision'=>8,'nai'=>'@@000000021'}
        }
      }]
    }


    signing_keys = keys.call['recovery_private']
    signed = Xgt::Ruby::Auth.sign_transaction(rpc, txn, [signing_keys], chain_id)
    $stderr.puts(%(Registering witness with recovery private WIF "#{keys.call['recovery_private']}"...))
    $stderr.puts(%(Signing keypair is #{keys.call['witness_private']} (private) and #{keys.call['witness_public']} (public)...))
    response = rpc.call('transaction_api.broadcast_transaction', [signed])
    $stderr.puts(%(Registered witness #{name}))
  end

  desc 'Assuming keys were generated, do everything else'
  task :all => [:create_wallet, :register]

  desc 'Regenerate keys and do everything else'
  task :really_all => [:generate_keys, :create_wallet, :register]
end

namespace :contracts do
  desc 'Generate a sample contract'
  task :generate do
    txn = {
      'extensions' => [],
      'operations' => [
        {
          'type' => 'contract_create_operation',
          'value' => {
            'owner' => wallet,
            'code' => "600260030100",
          }
        }
      ]
    }
    signed = Xgt::Ruby::Auth.sign_transaction(rpc, txn, [wif], chain_id)
    $stderr.puts(%(Registering contract... #{signed.to_json}))
    response = rpc.call('transaction_api.broadcast_transaction', [signed])
    $stderr.puts(%(Received response contract... #{response}))
  end

  desc 'View sample contracts'
  task :list do
    response = rpc.call('contract_api.list_owner_contracts', { 'owner' => wallet }) || {}
    p response
  end
end
