P(0x00, P_KEEPALIVE,          "i"         )
P(0x01, P_LOGIN,              "izziiccc"  )
P(0x02, P_HANDSHAKE,          "z"         )
P(0x03, P_CHAT_MSG,           "z"         )
P(0x04, P_TIME_CHANGE,        "l"         )
P(0x05, P_EQUIP_CHANGE,       "isss"      )
P(0x06, P_SPAWN_POS,          "iii"       )
P(0x07, P_USE_ENTITY,         "ic"        )
P(0x08, P_HEALTH_CHANGE,      "ssf"       )
P(0x09, P_RESPAWN,            "iccsz"     )
P(0x0A, P_PLAYER_FLYING,      "c"         )
P(0x0B, P_PLAYER_POS,         "ddddc"     )
P(0x0C, P_PLAYER_LOOK,        "ffc"       )
P(0x0D, P_PLAYER_POS_LOOK,    "ddddffc"   )
P(0x0E, P_PLAYER_DIG,         "cicic"     )
P(0x0F, P_PLAYER_BUILD,       "icicscs"   )
P(0x10, P_HOLDING_CHANGE,     "s"         )
P(0x11, P_USE_BED,            "icici"     )
P(0x12, P_ANIMATION,          "ic"        )
P(0x13, P_ENTITY_ACTION,      "ic"        )
P(0x14, P_PLAYER_SPAWN,       "iziiiccs"  )
P(0x15, P_ITEM_SPAWN,         "iscsiiiccc")
P(0x16, P_ITEM_COLLECT,       "ii"        )
P(0x17, P_ENTITY_SPAWN,       "iciiiisss" )
P(0x18, P_MOB_SPAWN,          "iciiicccm" )
P(0x19, P_PAINTING,           "iziiii"    )
P(0x1A, P_XP_ORB,             "iiiis"     )

P(0x1C, P_ENTITY_VELOCITY,    "isss"      )
P(0x1D, P_ENTITY_DESTROY,     "i"         )
P(0x1E, P_ENTITY,             "i"         )
P(0x1F, P_ENTITY_MOVE_REL,    "iccc"      )
P(0x20, P_ENTITY_LOOK,        "icc"       )
P(0x21, P_ENTITY_MR_LOOK,     "iccccc"    )
P(0x22, P_ENTITY_TELEPORT,    "iiiicc"    )
P(0x23, P_ENTITY_HEAD,        "ic"        )

P(0x26, P_ENTITY_STATUS,      "ic"        )
P(0x27, P_ENTITY_ATTACH,      "ii"        )
P(0x28, P_ENTITY_DATA,        "im"        )
P(0x29, P_ENTITY_FX_START,    "iccs"      )
P(0x2A, P_ENTITY_FX_STOP,     "ic"        )
P(0x2B, P_EXPERIENCE,         "fss"       )
    
P(0x32, P_CHUNK_PRELOAD,      "iic"       )
P(0x33, P_CHUNK_LOAD,         "iicss!"    )
P(0x34, P_CHUNK_DIFF,         "iis!"      )
P(0x35, P_BLOCK_CHANGE,       "icicc"     )
P(0x36, P_BLOCK_ACTION,       "isicc"     )
   
P(0x3C, P_EXPLOSION,          "dddf!"     )
P(0x3D, P_SOUND_EFFECT,       "iicii"     )

P(0x46, P_STATE_CHANGE,       "cc"        )
P(0x47, P_LIGHTNING,          "iciii"     )
  
P(0x64, P_WINDOW_OPEN,        "cczc"      )
P(0x65, P_WINDOW_CLOSE,       "c"         )
P(0x66, P_WINDOW_CLICK,       "cscscscs"  )
P(0x67, P_SLOT_CHANGE,        "cs!"       )
P(0x68, P_SLOTS_CHANGE,       "c!"        )
P(0x69, P_PROGRESS_BAR,       "css"       )
P(0x6A, P_TRANSACTION,        "csc"       )
P(0x6B, P_CREATIVE_GIVE,      "ssss"      )
   
P(0x82, P_SIGN_CHANGE,        "isizzzz"   )
P(0x83, P_MAP_DATA,           "ss!"       )
P(0x84, P_TILE_ENTITY,        "isiciii"   )

P(0xC8, P_STATISTIC,          "ic"        )
P(0xC9, P_PLAYER_LIST,        "zcs"       )
P(0xCA, P_PLAYER_PRIVS,       "cccc"      )

P(0xFA, P_MOD_MSG,            "z!"        )
     
P(0xFE, P_SERVER_PING,        ""          )
P(0xFF, P_DISCONNECT,         "z"         )