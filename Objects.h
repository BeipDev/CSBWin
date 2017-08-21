#ifndef __CSB_WIN_OBJECTS_H__
#define __CSB_WIN_OBJECTS_H__

enum SCROLLTYPE 
{
  scroll_OpenScroll = 0
};

enum  CHESTTYPE
{
  chest_Chest      = 0
};

enum POTIONTYPE
{
  potion_MonPotionA =  0,
  potion_UmPotion   =  1,
  potion_DesPotion  =  2,
  potion_VenPotion  =  3,
  potion_SarPotion  =  4,
  potion_ZoPotion   =  5,
  potion_RosPotion  =  6,
  potion_KuPotion   =  7,
  potion_DanePotion =  8,
  potion_NetaPotion =  9,
  potion_AntiVenin  = 10,
  potion_MonPotionB = 11,
  potion_YaPotion   = 12,
  potion_EEPotion   = 13,
  potion_ViPotion   = 14,
  potion_WaterFlask = 15,
  potion_KathBomb   = 16,
  potion_PewBomb    = 17,
  potion_RaBomb     = 18,
  potion_FulBomb    = 19,
  potion_EmptyFlask = 20
};

enum WEAPONTYPE
{
  weapon_EyeOfTime    =  0,
  weapon_StormRing    =  1,
  weapon_Torch        =  2,
  weapon_Flamitt      =  3,
  weapon_StaffOfClaws =  4,
  weapon_Storm        =  5,
  weapon_RaBlade      =  6,
  weapon_TheFirestaffA=  7,
  weapon_Dagger       =  8,
  weapon_Falchion     =  9,
  weapon_Sword        = 10,
  weapon_Rapier       = 11,
  weapon_Biter        = 12,
  weapon_SamuraiSword = 13,
  weapon_SideSplitter = 14,
  weapon_DiamondEdge  = 15,
  weapon_VorpalBlade  = 16,
  weapon_DragonFang   = 17,
  weapon_Axe          = 18,
  weapon_Executioner  = 19,
  weapon_Mace         = 20,
  weapon_MaceOfOrder  = 21,
  weapon_Morningstar  = 22,
  weapon_Club         = 23,
  weapon_StoneClub    = 24,
  weapon_ClawBow      = 25,
  weapon_Crossbow     = 26,
  weapon_Arrow        = 27,
  weapon_Slayer       = 28,
  weapon_Sling        = 29,
  weapon_Rock         = 30,
  weapon_PoisonDart   = 31,
  weapon_ThrowingStar = 32,
  weapon_Stick        = 33,
  weapon_Staff        = 34,
  weapon_Wand         = 35,
  weapon_TeoWand      = 36,
  weapon_YewStaff     = 37,
  weapon_StaffOfIrra  = 38,
  weapon_CrossOfNeta  = 39,
  weapon_SerpentStaff = 40,
  weapon_DragonSpit   = 41,
  weapon_SceptreOfLyf = 42,
  weapon_HornOfFear   = 43,
  weapon_Speedbow     = 44,
  weapon_TheFirestaffB= 45,
  weapon_numTypes     = 46
};

enum CLOTHINGTYPE
{
  clothing_Cape          =  0,
  clothing_CloakOfNight  =  1,
  clothing_TatteredPants =  2,
  clothing_Sandals       =  3,
  clothing_LeatherBoots  =  4,
  clothing_TatteredShirt =  5,
  clothing_Robe          =  6,
  clothing_FineRobeA     =  7,
  clothing_FineRobeB     =  8,
  clothing_Kirtle        =  9,
  clothing_SilkShirt     = 10,
  clothing_Tabard        = 11,
  clothing_Gunna         = 12,
  clothing_ElvenDoublet  = 13,
  clothing_ElvenHuke     = 14,
  clothing_ElvenBoots    = 15,
  clothing_LeatherJerkin = 16,
  clothing_LeatherPants  = 17,
  clothing_SuedeBoots    = 18,
  clothing_BluePants     = 19,
  clothing_Tunic         = 20,
  clothing_Ghi           = 21,
  clothing_GhiTrousers   = 22,
  clothing_Calista       = 23,
  clothing_CrownOfNerra  = 24,
  clothing_BezerkerHelm  = 25,
  clothing_Helmet        = 26,
  clothing_Basinet       = 27,
  clothing_NetaShield    = 28,
  clothing_CrystalShield = 29,
  clothing_WoodenShield  = 30,
  clothing_SmallShield   = 31,
  clothing_MailAketon    = 32,
  clothing_LegMail       = 33,
  clothing_MithralAketon = 34,
  clothing_MithralMail   = 35,
  clothing_CasqueNCoif   = 36,
  clothing_Hosen         = 37,
  clothing_Armet         = 38,
  clothing_TorsoPlate    = 39,
  clothing_LegPlate      = 40,
  clothing_FootPlate     = 41,
  clothing_SarShield     = 42,
  clothing_HelmOfRa      = 43,
  clothing_PlateOfRa     = 44,
  clothing_PoleynOfRa    = 45,
  clothing_GreaveOfRa    = 46,
  clothing_ShieldOfRa    = 47,
  clothing_DragonHelm    = 48,
  clothing_DragonPlate   = 49,
  clothing_DragonPoleyn  = 50,
  clothing_DragonGreave  = 51,
  clothing_DragonShield  = 52,
  clothing_Dexhelm       = 53,
  clothing_Flamebain     = 54,
  clothing_PowerTowers   = 55,
  clothing_BootsOfSpeed  = 56,
  clothing_Halter        = 57,
  clothing_numTypes      = 58
};

enum MONSTERTYPE
{
  mon_Scorpion       = 0,   //0
  mon_SlimeDevil     = 1,   //1
  mon_Giggler        = 2,  //2
  mon_FlyingEye      = 3,//3
  mon_Hellhound      = 4,//4
  mon_5              = 5,//5
  mon_Screamer       = 6,//6
  mon_RockPile       = 7,//7
  mon_Rive           = 8,//8
  mon_StoneGolem     = 9,//9
  mon_Mummy          = 0x0a,//10
  mon_BlackFlame     = 0x0b,//11
  mon_Skeleton       = 0x0c,//12
  mon_Couatl         = 0x0d,//13
  mon_Vexirk         = 0x0e,//14
  mon_Worm           = 0x0f,//15
  mon_AntMan         = 0x10,//16
  mon_Muncher        = 0x11,//17
  mon_DethKnight     = 0x12,//18
  mon_Zytaz          = 0x13,//19
  mon_WaterElemental = 0x14,//20
  mon_Oitu           = 0x15,//21
  mon_Demon          = 0x16,//22
  mon_LordChaos      = 0x17,//23
  mon_Dragon         = 0x18,//24
  mon_25             = 0x19,
  mon_GreyLord       = 0x1a,
  mon_undefined      = 0x63
};

enum MISCTYPE
{
 misc_Compass        = 0,
 misc_Waterskin      = 1,
 misc_JewelSymal     = 2,
 misc_Illumulet      = 3,
 misc_Ashes          = 4,
 misc_BonesOfHero    = 5, //value = character index
 misc_SarCoin        = 6,
 misc_SilverCoin     = 7,
 misc_GorCoin        = 8,
 misc_IronKey        = 9,
 misc_KeyOfB         = 10,
 misc_SolidKey       = 11,
 misc_SquareKey      = 12,
 misc_TourquoiseKey  = 13,
 misc_CrossKey       = 14,
 misc_OnyxKey        = 15,
 misc_SkeletonKey    = 16,
 misc_GoldKey        = 17,
 misc_WingedKey      = 18,
 misc_TopazKey       = 19,
 misc_SapphireKey    = 20,
 misc_EmeraldKey     = 21,
 misc_RubyKey        = 22,
 misc_RaKey          = 23,
 misc_MasterKey      = 24,
 misc_Boulder        = 25,
 misc_BlueGem        = 26,
 misc_OrangeGem      = 27,
 misc_GreenGem       = 28,
 misc_Apple          = 29,
 misc_Corn           = 30,
 misc_Bread          = 31,
 misc_Cheese         = 32,
 misc_ScreamerSlice  = 33,
 misc_WormRound      = 34,
 misc_Shank          = 35,
 misc_DragonSteak    = 36,
 misc_GemOfAges      = 37,
 misc_EkkhardCross   = 38,
 misc_Moonstone      = 39,
 misc_TheHellion     = 40,
 misc_PendantFeral   = 41,
 misc_MagicalBoxA    = 42,
 misc_MagicalBoxB    = 43,
 misc_MirrorOfDawn   = 44,
 misc_Rope           = 45,
 misc_RabbitsFoot    = 46,
 misc_Corbum         = 47,
 misc_Choker         = 48,
 misc_LockPicks      = 49,
 misc_Magnifier      = 50,
 misc_ZokathraSpell  = 51,
 misc_Bones          = 52,
 misc_numTypes       = 53
};


enum OBJ_DESC_INDEX
{
                 //    = scroll type   +  0 //  1 Scroll type
  objDI_Scroll      = 0,
                 //    = chest type    +  1 //  1 Chest types
  objDI_Chest       = 1,
                 //    = potion type   +  2 // 21 Potion types
  objDI_MonPotionA = 2,
  objDI_UmPotion   = 3,
  objDI_DesPotion  = 4,
  objDI_VenPotion  = 5,
  objDI_SarPotion  = 6,
  objDI_ZoPotion   = 7,
  objDI_RosPotion  = 8,
  objDI_KuPotion   = 9,
  objDI_DanePotion = 10,
  objDI_NetaPotion = 11,
  objDI_AntiVenin  = 12,
  objDI_MonPotionB = 13,
  objDI_YaPotion   = 14,
  objDI_EEPotion   = 15,
  objDI_ViPotion   = 16,
  objDI_WaterFlask = 17,
  objDI_KathBomb   = 18,
  objDI_PewBomb    = 19,
  objDI_RaBomb     = 20,
  objDI_FulBomb    = 21,
  objDI_EmptyFlask = 22,
                 //    = weapon type   + 23 // 46 Weapon types
  objDI_EyeOfTime    =  23,
  objDI_StormRing    =  24,
  objDI_Torch        =  25,
  objDI_Flamitt      =  26,
  objDI_StaffOfClaws =  27,
  objDI_Storm        =  28,
  objDI_RaBlade      =  29,
  objDI_TheFirestaffA=  30,
  objDI_Dagger       =  31,
  objDI_Falchion     =  32,
  objDI_Sword        =  33,
  objDI_Rapier       =  34,
  objDI_Biter        =  35,
  objDI_SamuraiSword =  36,
  objDI_SideSplitter =  37,
  objDI_DiamondEdge  =  38,
  objDI_VorpalBlade  =  39,
  objDI_DragonFang   =  40,
  objDI_Axe          =  41,
  objDI_Executioner  =  42,
  objDI_Mace         =  43,
  objDI_MaceOfOrder  =  44,
  objDI_Morningstar  =  45,
  objDI_Club         =  46,
  objDI_StoneClub    =  47,
  objDI_ClawBow      =  48,
  objDI_Crossbow     =  49,
  objDI_Arrow        =  50,
  objDI_Slayer       =  51,
  objDI_Sling        =  52,
  objDI_Rock         =  53,
  objDI_PoisonDart   =  54,
  objDI_ThrowingStar =  55,
  objDI_Stick        =  56,
  objDI_Staff        =  57,
  objDI_Wand         =  58,
  objDI_TeoWand      =  59,
  objDI_YewStaff     =  60,
  objDI_StaffOfIrra  =  61,
  objDI_CrossOfNeta  =  62,
  objDI_SerpentStaff =  63,
  objDI_DragonSpit   =  64,
  objDI_SceptreOfLyf =  65,
  objDI_HornOfFear   =  66,
  objDI_Speedbow     =  67,
  objDI_TheFirestaffB=  68,
                 //    = clothing type + 69 // 58 Clothing types
  objDI_Cape          = 69,
  objDI_CloakOfNight  = 70,
  objDI_TatteredPants = 71,
  objDI_Sandals       = 72,
  objDI_LeatherBoots  = 73,
  objDI_TatteredShirt = 74,
  objDI_Robe          = 75,
  objDI_FineRobeA     = 76,
  objDI_FineRobeB     = 77,
  objDI_Kirtle        = 78,
  objDI_SilkShirt     = 79,
  objDI_Tabard        = 80,
  objDI_Gunna         = 81,
  objDI_ElvenDoublet  = 82,
  objDI_ElvenHuke     = 83,
  objDI_ElvenBoots    = 84,
  objDI_LeatherJerkin = 85,
  objDI_LeatherPants  = 86,
  objDI_SuedeBoots    = 87,
  objDI_BluePants     = 88,
  objDI_Tunic         = 89,
  objDI_Ghi           = 90,
  objDI_GhiTrousers   = 91,
  objDI_Calista       = 92,
  objDI_CrownOfNerra  = 93,
  objDI_BezerkerHelm  = 94,
  objDI_Helmet        = 95,
  objDI_Basinet       = 96,
  objDI_NetaShield    = 97,
  objDI_CrystalShield = 98,
  objDI_WoodenShield  = 99,
  objDI_SmallShield   = 100,
  objDI_MailAketon    = 101,
  objDI_LegMail       = 102,
  objDI_MithralAketon = 103,
  objDI_MithralMail   = 104,
  objDI_CasqueNCoif   = 105,
  objDI_Hosen         = 106,
  objDI_Armet         = 107,
  objDI_TorsoPlate    = 108,
  objDI_LegPlate      = 109,
  objDI_FootPlate     = 110,
  objDI_SarShield     = 111,
  objDI_HelmOfRa      = 112,
  objDI_PlateOfRa     = 113,
  objDI_PoleynOfRa    = 114,
  objDI_GreaveOfRa    = 115,
  objDI_ShieldOfRa    = 116,
  objDI_DragonHelm    = 117,
  objDI_DragonPlate   = 118,
  objDI_DragonPoleyn  = 119,
  objDI_DragonGreave  = 120,
  objDI_DragonShield  = 121,
  objDI_Dexhelm       = 122,
  objDI_Flamebain     = 123,
  objDI_PowerTowers   = 124,
  objDI_BootsOfSpeed  = 125,
  objDI_Halter        = 126,
                 //    = food type     +127 // 53 Misc types
  objDI_Compass        = 127,
  objDI_Waterskin      = 128,
  objDI_JewelSymal     = 129,
  objDI_Illumulet      = 130,
  objDI_Ashes          = 131,
  objDI_BonesOfHero    = 132, //value = character index
  objDI_SarCoin        = 133,
  objDI_SilverCoin     = 134,
  objDI_GorCoin        = 135,
  objDI_IronKey        = 136,
  objDI_KeyOfB         = 137,
  objDI_SolidKey       = 138,
  objDI_SquareKey      = 139,
  objDI_TourquoiseKey  = 140,
  objDI_CrossKey       = 141,
  objDI_OnyxKey        = 142,
  objDI_SkeletonKey    = 143,
  objDI_GoldKey        = 144,
  objDI_WingedKey      = 145,
  objDI_TopazKey       = 146,
  objDI_SapphireKey    = 147,
  objDI_EmeraldKey     = 148,
  objDI_RubyKey        = 149,
  objDI_RaKey          = 150,
  objDI_MasterKey      = 151,
  objDI_Boulder        = 152,
  objDI_BlueGem        = 153,
  objDI_OrangeGem      = 154,
  objDI_GreenGem       = 155,
  objDI_Apple          = 156,
  objDI_Corn           = 157,
  objDI_Bread          = 158,
  objDI_Cheese         = 159,
  objDI_ScreamerSlice  = 160,
  objDI_WormRound      = 161,
  objDI_Shank          = 162,
  objDI_DragonSteak    = 163,
  objDI_GemOfAges      = 164,
  objDI_EkkhardCross   = 165,
  objDI_Moonstone      = 166,
  objDI_TheHellion     = 167,
  objDI_PendantFeral   = 168,
  objDI_MagicalBoxA    = 169,
  objDI_MagicalBoxB    = 170,
  objDI_MirrorOfDawn   = 171,
  objDI_Rope           = 172,
  objDI_RabbitsFoot    = 173,
  objDI_Corbum         = 174,
  objDI_Choker         = 175,
  objDI_LockPicks      = 176,
  objDI_Magnifier      = 177,
  objDI_ZokathraSpell  = 178,
  objDI_Bones          = 179,
  objDI_NotAnObject    = 255
};


enum OBJ_NAME_INDEX
{
  objNI_first     = 0,
  objNI_Compass_N = 0,  //0  0
    objNI_FirstModifiableObject = objNI_Compass_N,
  objNI_Compass_E,      //1  1
  objNI_Compass_S,      //2  2
  objNI_Compass_W,      //3  3
  objNI_Torch_a,        //4  4
  objNI_Torch_b,        //5  5
  objNI_Torch_c,        //6  6
  objNI_Torch_d,        //7  7
  objNI_Waterskin,      //8  8
  objNI_Water,          //9  9
  objNI_JewelSymal_a,   //0a  10
  objNI_JewelSymal_b,   //0b  11
  objNI_Illumulet_a,    //0c  12
  objNI_Illumulet_b,    //0d
  objNI_Flamitt_a,      //0e
  objNI_Flamitt_b,      //0f
  objNI_EyeOfTime_a,    //10
  objNI_EyeOfTime_b,    //11
  objNI_StormRing_a,    //12
  objNI_StormRing_b,    //13
  objNI_StaffOfClaws_a, //14  20
  objNI_StaffOfClaws_b, //15  21
  objNI_StaffOfClaws_c, //16  22
  objNI_Storm_a,        //17
  objNI_Storm_b,        //18
  objNI_RABlade_a,      //19
  objNI_RABlade_b,      //1a
  objNI_TheFirestaff_a, //1b
  objNI_TheFirestaff_b, //1c
  objNI_TheFirestaff_c, //1d
  objNI_OpenScroll,     //1e
  objNI_Scroll,         //1f
    objNI_LastModifiableObject = objNI_Scroll,
  objNI_Dagger,         //20  32
  objNI_Falchion,       //21  33
  objNI_Sword,          //22  34
  objNI_Rapier,         //23  35
  objNI_Biter,          //24  36
  objNI_SamuraiSword,   //25  37
  objNI_SideSplitter,   //26  38
  objNI_DiamondEdge,    //27  39
  objNI_VorpalBlade,    //28  40
  objNI_DragonFang,     //29  41
  objNI_Axe,            //2a
  objNI_Executioner,    //2b
  objNI_Mace,           //2c
  objNI_MaceOfOrder,    //2d  45
  objNI_Morningstar,    //2e
  objNI_Club,           //2f
  objNI_StoneClub,      //30
  objNI_ClawBow,        //31
  objNI_Crossbow,       //32
  objNI_Arrow,          //33
  objNI_Slayer,         //34
  objNI_Sling,          //35
  objNI_Rock,           //36
  objNI_PoisonDart,     //37
  objNI_ThrowingStar,   //38
  objNI_Stick,          //39
  objNI_Staff,          //3a  58
  objNI_Wand,           //3b  59
  objNI_TeoWand,        //3c  60
  objNI_YewStaff,       //3d  61
  objNI_StaffOfIrra,    //3e  62
  objNI_CrossOfNeta,    //3f  63
  objNI_SerpentStaff,   //40  64
  objNI_DragonSpit,     //41  65
  objNI_SceptreOfLyf,   //42  66
  objNI_TatteredShirt,  //43
  objNI_FineRobe_a,     //44
  objNI_Kirtle,         //45
  objNI_SilkShirt,      //46
  objNI_ElvenDoublet,   //47
  objNI_LeatherJerkin,  //48
  objNI_Tunic,          //49
  objNI_Ghi,            //4a
  objNI_MailAketon,     //4b
  objNI_MithralAketon,  //4c
  objNI_TorsoPlate,     //4d
  objNI_PlateOfRa,      //4e
  objNI_DragonPlate,    //4f
  objNI_Cape,           //50 80
  objNI_CloakOfNight,   //51 81
  objNI_TatteredPants,  //52 82
  objNI_Robe,           //53 83
  objNI_FineRobe_b,     //54 84
  objNI_Tabard,         //55 85
  objNI_Gunna,          //56
  objNI_ElvenHuke,      //57
  objNI_LeatherPants,   //58
  objNI_BluePants,      //59
  objNI_GhiTrousers,    //5a
  objNI_LegMail,        //5b
  objNI_MithralMail,    //5c
  objNI_LegPlate,       //5d
  objNI_PoleynOfRa,     //5e
  objNI_DragonPoleyn,   //5f
  objNI_BezerkerHelm,   //60
  objNI_Helmet,         //61
  objNI_Basinet,        //62
  objNI_CasqueNCoif,    //63
  objNI_Armet,          //64 100
  objNI_HelmOfRa,       //65 101
  objNI_DragonHelm,     //66 102
  objNI_Calista,        //67 103
  objNI_CrownOfNerra,   //68 104
  objNI_NetaShield,     //69
  objNI_CrystalShield,  //6a
  objNI_SmallShield,    //6b
  objNI_WoodenShield,   //6c 108
  objNI_SarShield,      //6d
  objNI_ShieldOfRa,     //6e
  objNI_DragonShield,   //6f
  objNI_Sandals,        //70
  objNI_SuedeBoots,     //71
  objNI_LeatherBoots,   //72
  objNI_Hosen,          //73
  objNI_FootPlate,      //74
  objNI_GreaveOfRa,     //75
  objNI_DragonGreave,   //76
  objNI_ElvenBoots,     //77
  objNI_GemOfAges,      //78
  objNI_EkkhardCross,   //79
  objNI_Moonstone,      //7a 122
  objNI_TheHellion,     //7b 123
  objNI_PendantFeral,   //7c 124
  objNI_SarCoin,        //7d 125
  objNI_SilverCoin,     //7e 126
  objNI_GorCoin,        //7f 127
  objNI_Boulder,        //80 128
  objNI_BlueGem,        //81 129
  objNI_OrangeGem,      //82 130
  objNI_GreenGem,       //83 131
  objNI_MagicalBox_a,   //84 132
  objNI_MagicalBox_b,   //85 133
  objNI_MirrorOfDawn,   //86 134
  objNI_HornOfFear,     //87 135
  objNI_Rope,           //88 136
  objNI_RabbitsFoot,    //89 137
  objNI_Corbum,         //8a 138
  objNI_Choker,         //8b 139
  objNI_Dexhelm,        //8c 140
  objNI_Flamebain,      //8d 141
  objNI_Powertowers,    //8e 142 
  objNI_Speedbow,       //8f
  objNI_Chest,          //90
  objNI_OpenChest,      //91
  objNI_Ashes,          //92
  objNI_Bones_a,        //93
  objNI_MonPotion_a,    //94
    objNI_FirstFullFlask = objNI_MonPotion_a,
  objNI_UmPotion,       //95
  objNI_DesPotion,      //96
  objNI_VenPotion,      //97
  objNI_SarPotion,      //98
  objNI_ZoPotion,       //99
  objNI_RosPotion,      //9a
  objNI_KuPotion,       //9b
  objNI_DanePotion,     //9c
  objNI_NetaPotion,     //9d
  objNI_AntiVenin,      //9e
  objNI_MonPotion_b,    //9f
  objNI_YaPotion,       //a0 160
  objNI_EePotion,       //a1
  objNI_ViPotion,       //a2
  objNI_WaterFlask,     //a3
    objNI_LastFullFlask = objNI_WaterFlask,
  objNI_KathBomb,       //a4
  objNI_PewBomb,        //a5
  objNI_RaBomb,         //a6
  objNI_FulBomb,        //a7
  objNI_Apple,          //a8  168
  objNI_Corn,           //a9  169
  objNI_Bread,          //aa  170
  objNI_Cheese,         //ab  171
  objNI_ScreamerSlice,  //ac  172
  objNI_WormRound,      //ad  173
  objNI_Shank,          //ae  174
  objNI_DragonSteak,    //af  175
  objNI_IronKey,        //b0  176
    objNI_FirstKey = objNI_IronKey,
  objNI_KeyOfB,         //b1  177  
  objNI_SolidKey,       //b2  178
  objNI_SquareKey,      //b3  179
  objNI_TourquoiseKey,  //b4
  objNI_CrossKey,       //b5
  objNI_OnyxKey,        //b6
  objNI_SkeletonKey,    //b7
  objNI_GoldKey,        //b8
  objNI_WingedKey,      //b9
  objNI_TopazKey,       //ba
  objNI_SapphireKey,    //bb
  objNI_EmeraldKey,     //bc
  objNI_RubyKey,        //bd
  objNI_RaKey,          //be
  objNI_MasterKey,      //bf
    objNI_LastKey = objNI_MasterKey,
  objNI_LockPicks,      //c0 192
  objNI_Magnifier,      //c1
  objNI_BootsOfSpeed,   //c2
  objNI_EmptyFlask,     //c3
  objNI_Halter,         //c4
  objNI_ZokathraSpell,  //c5
  objNI_Bones_b,        //c6 198
  objNI_Special_a,      //c7 199
  objNI_Special_b,      //c8 200
  objNI_Special_c,      //c9 201
  objNI_Special_d,      //ca 202
  objNI_Special_e,      //cb 203 
  objNI_Special_f,      //cc 204
  objNI_Special_g,      //cd 205
  objNI_Special_h,      //ce 206
  objNI_Special_i,      //cf 207
  objNI_Special_j,      //d0 208
  objNI_Special_k,      //d1
  objNI_Special_l,      //d2
  objNI_Special_m,      //d3
  objNI_Special_n,      //d4
  objNI_last  = objNI_Special_n,
  objNI_NotAnObject = 0xff
};

#endif
