/*
 * Copyright (c) 2021 Gather.com, Inc., and contributors.
 * Copyright (c) 2016 Steemit, Inc., and contributors.
 */
#pragma once
#include <xgt/protocol/hardfork.hpp>

// WARNING!
// Every symbol defined here needs to be handled appropriately in get_config.cpp
// This is checked by get_config_check.sh called from Dockerfile

#define XGT_BLOCKCHAIN_VERSION              ( version(0, 0, 0) )

#define XGT_ADDRESS_PREFIX                  "XGT"

#ifdef IS_TEST_NET
#define XGT_CHAIN_ID                        fc::sha256::hash("testnet")
#else
#define XGT_CHAIN_ID                        fc::sha256::hash("xgt")
#endif

#define XGT_WALLET_NAME_LENGTH              40 /// NOTE: If you change this, change XGT_INIT_MINER_NAME also.

#ifdef IS_TEST_NET
#define XGT_NETWORK_TYPE                    "testnet"
#else
#define XGT_NETWORK_TYPE                    "mainnet"
#endif

#ifdef IS_TEST_NET
#define XGT_INIT_PRIVATE_KEY (fc::ecc::private_key::regenerate(fc::sha256::hash(std::string("init_key"))))
#define XGT_INIT_PUBLIC_KEY_STR (std::string( xgt::protocol::public_key_type(XGT_INIT_PRIVATE_KEY.get_public_key()) ))
#else
#define XGT_INIT_PUBLIC_KEY_STR             "XGT7dDoJbrmueAw431pPbjLDoRhqFCC5Xs5o6f1cZLepWEpkcy3Tc"
#endif

#ifdef IS_TEST_NET

// #define XGT_GENESIS_TIME                    (fc::time_point_sec(1576564673))
#define XGT_GENESIS_TIME                    (fc::time_point_sec(1586840015))
#define XGT_UPVOTE_LOCKOUT_SECONDS          (60*5)    /// 5 minutes

#define XGT_MIN_WALLET_CREATION_FEE          0
#define XGT_MAX_WALLET_CREATION_FEE          0

#define XGT_RECOVERY_AUTH_RECOVERY_PERIOD                  fc::seconds(60)
#define XGT_WALLET_RECOVERY_REQUEST_EXPIRATION_PERIOD     fc::seconds(12)
#define XGT_RECOVERY_UPDATE_LIMIT                          fc::seconds(0)
#define XGT_RECOVERY_AUTH_HISTORY_TRACKING_START_BLOCK_NUM 1

#else // IS LIVE XGT NETWORK

// #define XGT_GENESIS_TIME                    (fc::time_point_sec(1576564673))
#define XGT_GENESIS_TIME                    (fc::time_point_sec(1586840015))
#define XGT_UPVOTE_LOCKOUT_SECONDS          (60*60*12)    /// 12 hours

#define XGT_MIN_WALLET_CREATION_FEE           0
#define XGT_MAX_WALLET_CREATION_FEE           0

#define XGT_RECOVERY_AUTH_RECOVERY_PERIOD                  fc::days(30)
#define XGT_WALLET_RECOVERY_REQUEST_EXPIRATION_PERIOD     fc::days(1)
#define XGT_RECOVERY_UPDATE_LIMIT                          fc::minutes(60)
#define XGT_RECOVERY_AUTH_HISTORY_TRACKING_START_BLOCK_NUM 3186477

#endif

#define XGT_INIT_SUPPLY                     (int64_t( 1872936875000000 )) // 1,872,936,875,000,000 G

/// Version format string.  The Xgt binary will refuse to load a state file where this does not match the built-in version.
#define XGT_DB_FORMAT_VERSION               "1"

#define XGT_SYMBOL    (xgt::protocol::asset_symbol_type::from_asset_num( XGT_ASSET_NUM_XGT ) )

#define XGT_BLOCKCHAIN_HARDFORK_VERSION     ( hardfork_version( XGT_BLOCKCHAIN_VERSION ) )

#define XGT_100_PERCENT                     10000
#define XGT_1_PERCENT                       (XGT_100_PERCENT/100)

#define XGT_BLOCK_INTERVAL                  5

#define XGT_BLOCKS_PER_YEAR                 (365*24*60*60/XGT_BLOCK_INTERVAL)
#define XGT_BLOCKS_PER_DAY                  (24*60*60/XGT_BLOCK_INTERVAL)
#define XGT_START_MINER_VOTING_BLOCK        (XGT_BLOCKS_PER_DAY * 30)

#define XGT_INIT_MINER_NAME                 "XGT0000000000000000000000000000000000000000"
#define XGT_INIT_TIME                       (fc::time_point_sec());

// TODO: Change these!
#ifndef XGT_MAX_WITNESSES
#define XGT_MAX_WITNESSES 10
#endif
#ifndef XGT_MAX_VOTED_WITNESSES
#define XGT_MAX_VOTED_WITNESSES 0
#endif
#ifndef XGT_MAX_MINER_WITNESSES
#define XGT_MAX_MINER_WITNESSES 10
#endif

// TODO: Look into this logic
#ifdef IS_TEST_NET
#define XGT_HARDFORK_REQUIRED_WITNESSES     1
#else
#define XGT_HARDFORK_REQUIRED_WITNESSES     17 // 17 of the 21 dpos witnesses (20 elected and 1 virtual time) required for hardfork. This guarantees 75% participation on all subsequent rounds.
#endif
#define XGT_MAX_TIME_UNTIL_EXPIRATION       (60*60) // seconds,  aka: 1 hour
#define XGT_MAX_MEMO_SIZE                   2048
#define XGT_VOTING_ENERGY_REGENERATION_SECONDS (1*60*60*24) // 1 day
#define XGT_ENERGY_REGENERATION_SECONDS (1*60*60*24) // 1 day
#define XGT_MAX_VOTE_CHANGES                5
#define XGT_REVERSE_AUCTION_WINDOW_SECONDS (60*5) /// 5 minutes
#define XGT_MIN_VOTE_INTERVAL_SEC           3
#define XGT_VOTE_DUST_THRESHOLD             (50000000)
#define XGT_DOWNVOTE_POOL_PERCENT_HF21      (25*XGT_1_PERCENT)

#define XGT_MIN_ROOT_COMMENT_INTERVAL       (fc::seconds(60*5)) // 5 minutes
#define XGT_MIN_REPLY_INTERVAL              (fc::seconds(20)) // 20 seconds
#define XGT_MIN_REPLY_INTERVAL_HF20         (fc::seconds(3)) // 3 seconds
#define XGT_MIN_COMMENT_EDIT_INTERVAL       (fc::seconds(3)) // 3 seconds
#define XGT_POST_AVERAGE_WINDOW             (60*60*24u) // 1 day
#define XGT_POST_WEIGHT_CONSTANT            (uint64_t(4*XGT_100_PERCENT) * (4*XGT_100_PERCENT))// (4*XGT_100_PERCENT) -> 2 posts per 1 days, average 1 every 12 hours

#define XGT_INFLATION_RATE_START_PERCENT    (978) // Fixes block 7,000,000 to 9.5%
#define XGT_INFLATION_RATE_STOP_PERCENT     (95) // 0.95%
#define XGT_INFLATION_NARROWING_PERIOD      (250000) // Narrow 0.01% every 250k blocks

#define XGT_HF21_CONVERGENT_LINEAR_RECENT_CLAIMS (fc::uint128_t(0,503600561838938636ull))
#define XGT_CONTENT_CONSTANT_HF21           (fc::uint128_t(0,2000000000000ull))

#define XGT_MINER_PAY_PERCENT               (XGT_1_PERCENT) // 1%
#define XGT_MAX_RATION_DECAY_RATE           (1000000)

#define XGT_BANDWIDTH_AVERAGE_WINDOW_SECONDS (60*60*24*7) ///< 1 week
#define XGT_BANDWIDTH_PRECISION             (uint64_t(1000000)) ///< 1 million
#define XGT_MAX_COMMENT_DEPTH_PRE_HF17      6
#define XGT_MAX_COMMENT_DEPTH               0xffff // 64k
#define XGT_SOFT_MAX_COMMENT_DEPTH          0xff // 255

#define XGT_MAX_RESERVE_RATIO               (20000)

#define XGT_CREATE_WALLET_WITH_XGT_MODIFIER 30

#define XGT_MINING_BTC_BLOCKS_PER_SECOND    (1.0f / 600.0f)
#define XGT_MINING_BLOCKS_PER_SECOND        (1.0f / 4.0f)
#define XGT_MINING_RELATIVE_SPEED           (XGT_MINING_BLOCKS_PER_SECOND / XGT_MINING_BTC_BLOCKS_PER_SECOND)

#define XGT_STARTING_OFFSET                 0
#define XGT_MINING_REWARD                   asset( 4166166, XGT_SYMBOL )
#define XGT_MIN_POW_REWARD                  XGT_MINING_REWARD
#define XGT_MINING_ADJUSTMENT_MAX_FACTOR    4.0f
#define XGT_MINING_REWARD_HALVING_INTERVAL  uint64_t( 201000 * XGT_MINING_RELATIVE_SPEED )
#define XGT_MINING_RECALC_EVERY_N_BLOCKS    uint64_t( 2016 * XGT_MINING_RELATIVE_SPEED )
#define XGT_MINING_TARGET_START             "000000ffff000000000000000000000000000000000000000000000000000000"
#define XGT_MINING_TARGET_MAX               "00000ffff0000000000000000000000000000000000000000000000000000000"

#define XGT_ACTIVE_CHALLENGE_FEE            asset( 2000, XGT_SYMBOL )
#define XGT_RECOVERY_CHALLENGE_FEE          asset( 30000, XGT_SYMBOL )
#define XGT_ACTIVE_CHALLENGE_COOLDOWN       fc::days(1)
#define XGT_RECOVERY_CHALLENGE_COOLDOWN     fc::days(1)

#define XGT_CONTENT_CONSTANT_HF0            (uint128_t(uint64_t(2000000000000ll)))
// note, if redefining these constants make sure calculate_claims doesn't overflow

// 5ccc e802 de5f
// int(expm1( log1p( 1 ) / BLOCKS_PER_YEAR ) * 2**XGT_APR_PERCENT_SHIFT_PER_BLOCK / 100000 + 0.5)
// we use 100000 here instead of 10000 because we end up creating an additional 9x for vesting
#define XGT_APR_PERCENT_MULTIPLY_PER_BLOCK            ( (uint64_t( 0x5ccc ) << 0x20) \
                                                        | (uint64_t( 0xe802 ) << 0x10) \
                                                        | (uint64_t( 0xde5f )        ) \
                                                        )
// chosen to be the maximal value such that XGT_APR_PERCENT_MULTIPLY_PER_BLOCK * 2**64 * 100000 < 2**128
#define XGT_APR_PERCENT_SHIFT_PER_BLOCK               87

#define XGT_APR_PERCENT_MULTIPLY_PER_ROUND            ( (uint64_t( 0x79cc ) << 0x20 ) \
                                                        | (uint64_t( 0xf5c7 ) << 0x10 ) \
                                                        | (uint64_t( 0x3480 )         ) \
                                                        )

#define XGT_APR_PERCENT_SHIFT_PER_ROUND               83

// We have different constants for hourly rewards
// i.e. hex(int(math.expm1( math.log1p( 1 ) / HOURS_PER_YEAR ) * 2**XGT_APR_PERCENT_SHIFT_PER_HOUR / 100000 + 0.5))
#define XGT_APR_PERCENT_MULTIPLY_PER_HOUR             ( (uint64_t( 0x6cc1 ) << 0x20) \
                                                        | (uint64_t( 0x39a1 ) << 0x10) \
                                                        | (uint64_t( 0x5cbd )        ) \
                                                        )

// chosen to be the maximal value such that XGT_APR_PERCENT_MULTIPLY_PER_HOUR * 2**64 * 100000 < 2**128
#define XGT_APR_PERCENT_SHIFT_PER_HOUR                77

// These constants add up to GRAPHENE_100_PERCENT.  Each GRAPHENE_1_PERCENT is equivalent to 1% per year APY
// *including the corresponding 9x vesting rewards*
#define XGT_CURATE_APR_PERCENT              3875
#define XGT_CONTENT_APR_PERCENT             3875
#define XGT_PRODUCER_APR_PERCENT             750
#define XGT_POW_APR_PERCENT                  750

#define XGT_MIN_WALLET_NAME_LENGTH          3
#define XGT_MAX_WALLET_NAME_LENGTH         16

#define XGT_MIN_PERMLINK_LENGTH             0
#define XGT_MAX_PERMLINK_LENGTH             256
#define XGT_MAX_WITNESS_URL_LENGTH          2048

#define XGT_MAX_SHARE_SUPPLY                int64_t(1000000000000000ll)
#define XGT_MAX_SATOSHIS                    int64_t(4611686018427387903ll)
#define XGT_MAX_SIG_CHECK_DEPTH             2
#define XGT_MAX_SIG_CHECK_WALLETS          125

#define XGT_MIN_TRANSACTION_SIZE_LIMIT      1024
#define XGT_SECONDS_PER_YEAR                (uint64_t(60*60*24*365ll))

#define XGT_MAX_TRANSACTION_SIZE            (1024*64)
#define XGT_MIN_BLOCK_SIZE_LIMIT            (XGT_MAX_TRANSACTION_SIZE)
#define XGT_MAX_BLOCK_SIZE                  (XGT_MAX_TRANSACTION_SIZE*XGT_BLOCK_INTERVAL*2000)
#define XGT_SOFT_MAX_BLOCK_SIZE             (2*1024*1024)
#define XGT_MIN_BLOCK_SIZE                  115
#define XGT_BLOCKS_PER_HOUR                 (60*60/XGT_BLOCK_INTERVAL)
#define XGT_FEED_INTERVAL_BLOCKS            (XGT_BLOCKS_PER_HOUR)
#define XGT_MAX_FEED_AGE_SECONDS            (60*60*24*7) // 7 days
#define XGT_MIN_FEEDS                       (XGT_MAX_WITNESSES/3) /// protects the network from conversions before price has been established

#define XGT_MIN_UNDO_HISTORY                10
#define XGT_MAX_UNDO_HISTORY                151200 // 2016 * 150 * 0.5

#define XGT_MIN_TRANSACTION_EXPIRATION_LIMIT (XGT_BLOCK_INTERVAL * 5) // 5 transactions per block
#define XGT_BLOCKCHAIN_PRECISION            uint64_t( 100000000 )

#define XGT_BLOCKCHAIN_PRECISION_DIGITS     8
#define XGT_MAX_INSTANCE_ID                 (uint64_t(-1)>>16)
/** NOTE: making this a power of 2 (say 2^15) would greatly accelerate fee calcs */
#define XGT_MAX_AUTHORITY_MEMBERSHIP        40
#define XGT_MAX_ASSET_WHITELIST_AUTHORITIES 10
#define XGT_MAX_URL_LENGTH                  127

#define XGT_IRREVERSIBLE_THRESHOLD          (75 * XGT_1_PERCENT)

#define XGT_VIRTUAL_SCHEDULE_LAP_LENGTH  ( fc::uint128(uint64_t(-1)) )
#define XGT_VIRTUAL_SCHEDULE_LAP_LENGTH2 ( fc::uint128::max_value() )

#define XGT_INITIAL_VOTE_POWER_RATE (40)
#define XGT_REDUCED_VOTE_POWER_RATE (10)
#define XGT_VOTES_PER_PERIOD_XTT_HF (50)

#define XGT_MAX_LIMIT_ORDER_EXPIRATION     (60*60*24*28) // 28 days

#define XGT_RD_MIN_DECAY_BITS               6
#define XGT_RD_MAX_DECAY_BITS              32
#define XGT_RD_DECAY_DENOM_SHIFT           36
#define XGT_RD_MAX_POOL_BITS               64
#define XGT_RD_MAX_BUDGET_1                ((uint64_t(1) << (XGT_RD_MAX_POOL_BITS + XGT_RD_MIN_DECAY_BITS - XGT_RD_DECAY_DENOM_SHIFT))-1)
#define XGT_RD_MAX_BUDGET_2                ((uint64_t(1) << (64-XGT_RD_DECAY_DENOM_SHIFT))-1)
#define XGT_RD_MAX_BUDGET_3                (uint64_t( std::numeric_limits<int32_t>::max() ))
#define XGT_RD_MAX_BUDGET                  (int32_t( std::min( { XGT_RD_MAX_BUDGET_1, XGT_RD_MAX_BUDGET_2, XGT_RD_MAX_BUDGET_3 } )) )
#define XGT_RD_MIN_DECAY                   (uint32_t(1) << XGT_RD_MIN_DECAY_BITS)
#define XGT_RD_MIN_BUDGET                  1
#define XGT_RD_MAX_DECAY                   (uint32_t(0xFFFFFFFF))

#define XGT_DECAY_BACKSTOP_PERCENT         (90 * XGT_1_PERCENT)

#define XGT_BLOCK_GENERATION_POSTPONED_TX_LIMIT 5
#define XGT_PENDING_TRANSACTION_EXECUTION_LIMIT fc::milliseconds(200)

#define XGT_CUSTOM_OP_ID_MAX_LENGTH        (32)
#define XGT_CUSTOM_OP_DATA_MAX_LENGTH      (8192)
#define XGT_COMMENT_TITLE_LIMIT            (256)

/**
 *  Reserved Account IDs with special meaning
 */
///@{
/// Represents the current witnesses
#define XGT_MINER_WALLET                   "XGT0000000000000000000000000000000000000001"
/// Represents the canonical account with NO authority (nobody can access funds in null account)
#define XGT_NULL_WALLET                    "XGT0000000000000000000000000000000000000002"
/// Represents the canonical account with WILDCARD authority (anybody can access funds in temp account)
#define XGT_TEMP_WALLET                    "XGT0000000000000000000000000000000000000003"
/// Represents the canonical root post parent account
#define XGT_ROOT_POST_PARENT                (wallet_name_type())
/// Represents the account with NO authority which holds resources for payouts according to given proposals
#define XGT_TREASURY_WALLET                "XGT0000000000000000000000000000000000000004"
///@}


#define XTT_MAX_VOTABLE_ASSETS 2
#define XTT_UPVOTE_LOCKOUT                      (60*60*12)   /// 12 hours
#define XTT_EMIT_INDEFINITELY                   (std::numeric_limits<uint32_t>::max())
#define XTT_MAX_NOMINAL_VOTES_PER_DAY           (1000)
#define XTT_DEFAULT_VOTES_PER_REGEN_PERIOD      (50)
#define XTT_BALLAST_SUPPLY_PERCENT              (XGT_1_PERCENT/10)
#define XTT_MAX_ICO_TIERS                       (10)
