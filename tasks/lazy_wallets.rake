namespace :lazy_wallets do
  task :name_test do
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

    keys = generate_keys
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
