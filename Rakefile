require 'fileutils'
require 'tmpdir'
require 'uri'
require 'json'
require 'bigdecimal'
require 'shellwords'
require 'rake/testtask'
require'xgt/ruby'

def wallet
  ENV['XGT_WALLET'] || 'XGT0000000000000000000000000000000000000000'
end

def wif
  ENV['XGT_WIF'] || '5JNHfZYKGaomSFvd4NUdQ9qMcEAC43kujbfjueTHpVapX1Kzq2n'
end

def recovery_private_key
  ENV['XGT_RECOVERY_PRIVATE_KEY'] || '5JNHfZYKGaomSFvd4NUdQ9qMcEAC43kujbfjueTHpVapX1Kzq2n'
end

def host
  ENV['XGT_HOST'] || 'http://localhost:8751'
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
  config['XGT_ADDRESS_PREFIX']
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
  Xgt::Ruby::Rpc.new(host || 'http://localhost:8751')
end

desc 'Removes build artifacts'
task :clean do
  sh 'rm -rf ../xgt-build'
end

desc 'Runs CMake to prepare the project'
task :configure do
  addr_prefix = 'XGT' # TODO: Config variable
  recovery_public_key = Xgt::Ruby::Auth.wif_to_public_key(recovery_private_key, addr_prefix)
  xgt_compile_args = []
  xgt_compile_args << %(-DXGT_ADDRESS_PREFIX=#{addr_prefix})
  if recovery_private_key
    xgt_compile_args << %(-DXGT_INIT_PRIVATE_KEY=#{recovery_private_key})
    xgt_compile_args << %(-DXGT_INIT_PUBLIC_KEY_STR=#{recovery_public_key})
  end
  sh %(
    mkdir -p ../xgt-build \
      && cd ../xgt-build \
      && cmake -DCMAKE_BUILD_TYPE=Debug \
               -D CMAKE_CXX_COMPILER="ccache" \
               -D CMAKE_CXX_COMPILER_ARG1="g++" \
               -D CMAKE_C_COMPILER="ccache" \
               -D CMAKE_C_COMPILER_ARG1="gcc" \
               #{ xgt_compile_args.join(' ') } \
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
  plugins = %w(
    chain p2p webserver witness database_api transaction_api block_api
    wallet_by_key wallet_history wallet_history_api wallet_by_key_api
    contract_api
  )

  sh 'rm -rf ../xgt-build/testnet-data'
  sh 'mkdir -p ../xgt-build/testnet-data'

  # TODO: Needs revisiting
  their_host = if host
    uri = URI.parse(host)
    %(#{uri.host}:#{uri.port})
  else
    nil
  end

  my_host = '0.0.0.0'
  path = '../xgt-build/testnet-data'
  File.open(File.join(path, 'config.ini'), 'w') do |f|
    f.puts(unindent(%(
      log-console-appender = {"appender":"stderr","stream":"std_error"}
      log-file-appender = {"appender":"logfile","file":"logfile.log"}
      log-logger = {"name":"default","level":"debug","appender":"stderr"}
      log-logger = {"name":"default","level":"debug","appender":"logfile"}

      backtrace = yes
      plugin = #{plugins.join(' ')}

      shared-file-dir = "blockchain"
      shared-file-size = 12G
      p2p-endpoint = #{my_host}:#{2001 + instance_index}
      #{their_host ? %(p2p-seed-node = #{their_host}) : %(p2p-seed-node =)}
      webserver-http-endpoint = #{my_host}:#{8751 + instance_index * 2}

      miner = ["#{wallet}","#{wif}"]
      mining-threads = 1
      witness = "#{wallet}"
      private-key = #{recovery_private_key}
      mining-reward-key = #{recovery_private_key}

      enable-stale-production = true
      required-participation = 0
    )))
  end
  $stderr.puts(File.read('../xgt-build/testnet-data/config.ini'))

  sh %(cd ../xgt-build/testnet-data && ../programs/xgtd/xgtd --data-dir=.)
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
    p ['private_key', private_key]
    p ['public_key', public_key]

    response = rpc.call('wallet_by_key_api.generate_wallet_name', {
      'recovery_keys' => [public_key]
    })
    wallet_name = response['wallet_name']
    p wallet_name

    txn = {
      'extensions' => [],
      'operations' => [
         {
           'type' => 'transfer_operation',
           'value' => {
             'amount' => {
               'amount' => '1',
               'precision' =>  3,
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
    #p rpc.call('wallet_history_api.get_wallet_history', { 'wallet' => wallet_name })

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
      %w(recovery money social memo).each do |role|
        private_key = Xgt::Ruby::Auth.generate_wif(wallet, master, 'recovery')
        public_key = Xgt::Ruby::Auth.wif_to_public_key(private_key, address_prefix)
        ks["#{role}_private"] = private_key
        ks["#{role}_public"] = public_key
      end
      f.puts(JSON.pretty_generate(ks))
    end
  }

  keys = ->() {
    JSON.load(File.open('out/keys.json'))
  }

  wallet_name = ->() {
    key = keys.call['recovery_public']
    p key
    result = rpc.call('wallet_by_key_api.get_key_references', { 'keys' => [key] })
    result['wallets'].first.first
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
    name = wallet_name.call
    raise %(Matching wallet named "#{name}" already exists!) if name

    txn = {
      'extensions' => [],
      'operations' => [
        {
          'type' => 'wallet_create_operation',
          'value' => {
            'fee' => {
              'amount' => '0',
              'precision' =>  3,
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
    name = wallet_name.call
    raise %(Matching wallet named "#{name}" doesn't already exist!) unless name

    components = fee.split(' ')
    decimal = BigDecimal(components.first) * 1
    final_fee = decimal.truncate.to_s + '.' + sprintf('%03d', (decimal.frac * 1000).truncate) + ' ' + components.last

    raise 'Witness already registered!' if does_witness_exist.call

    txn = {
      'extensions' => [],
      'operations' => [{
        'type' => 'witness_update',
        'value' => {
          'owner' => name,
          'url' => 'http://test.host',
          'block_signing_key' => keys.call['recovery_public'],
          'props' => {
            'account_creation_fee' => fee,
          },
          'fee' => final_fee,
        }
      }]
    }
    signed = Xgt::Ruby::Auth.sign_transaction(rpc, txn, [keys.call['recovery_private']], chain_id)
    $stderr.puts(%(Registering witness with recovery private WIF "#{keys.call['recovery_private']}"...))
    $stderr.puts(%(Signing keypair is #{keys.call['witness_private']} (private) and #{keys.call['witness_public']} (public)...))
    response = rpc.call('transaction_api.broadcast_transaction', [signed])
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
