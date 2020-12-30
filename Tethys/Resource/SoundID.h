
#pragma once

namespace Tethys {

enum class SoundID : int {
  Bld_fal3     = 0,
  Bld_fal1     = 1,
  Bld_fal2     = 2,
  Bld_make     = 3,
  Ag_sel       = 4,
  Tokamsel     = 5,
  Pantheon     = 6,
  Commsel      = 7,
  Dirt         = 8,
  Fac_andr     = 9,
  Fac_cons     = 10,
  Fac_sel      = 11,
  //  Fac_sel  = 12,
  Gar_sel      = 13,
  Geo_sel      = 14,
  Gorf         = 15,
  Guardsel     = 16,
  Lab_3        = 17,
  Lab_1        = 18,
  Lab_2        = 19,
  Light        = 20,
  Mag_sel      = 21,
  Medisel      = 22,
  //  Dirt     = 23,
  Mine_1       = 24,
  Mine_2       = 25,
  Nur_sel      = 26,
  Observat     = 27,
  Rec_fac      = 28,
  Res1_sel     = 29,
  Res2_sel     = 30,
  Res3_sel     = 31,
  Robosel      = 32,
  Smelter1     = 33,
  //  Smelter1 = 34,
  //  Tokamsel = 35,
  Spaceprt     = 36,
  Storage1     = 37,
  Storage2     = 38,
  //  Tokamsel = 39,
  //  Pantheon = 40,
  Uni_sel      = 41,
  Consvhc      = 42,
  Dozr_sel     = 43,
  Evac_trn     = 44,
  Earthwrk     = 45,
  Robot4       = 46,
  Spidsel      = 47,
  Infantry     = 48,
  Repvhc       = 49,
  Robmine      = 50,
  Robsurv      = 51,
  Scout        = 52,
  Combhev      = 53,
  Comblit      = 54,
  Combmed      = 55,
  Truk_sel     = 56,
  Bld_exp      = 57,
  Uni_grad     = 58,
  Tokaexp      = 59,
  Geyser       = 60,
  Gar_doc      = 61,
  Doc_dor1     = 62,
  Doc_dor2     = 63,
  Doc_grab     = 64,
  Sulv         = 65,
  Robdep       = 66,
  Reprep       = 67,
  Consrep      = 68,
  Dump         = 69,
  Micwarn      = 70,
  Micfire      = 71,
  Bigexp1      = 72,
  Bigexp2      = 73,
  Bigexp3      = 74,
  Medexp1      = 75,
  Medexp2      = 76,
  Smalxp1      = 77,
  Smalxp2      = 78,
  Smalxp3      = 79,
  Quake        = 80,
  Tornado2     = 81,
  Thunder      = 82,
  Microbe      = 83,
  Fuse         = 84,
  Beep2        = 85,
  //  Beep2    = 86,
  Beep5        = 87,
  Bld_not      = 88,
  Beep6        = 89,
  Beep9        = 90,
  Beep8        = 91,
  //  Beep5    = 92,
  Message2     = 93,

  SavantBegin  = 94,
  Savnt212     = 94,    ///< "Structure completed"
  Savant2      = 95,    ///< "Research completed"
  Savant3      = 96,    ///< "Our colony is under attack"
  Savant4      = 97,    ///< "We are under attack"
  Savant6      = 98,    ///< "Vehicle ready"
  Savant7      = 99,    ///< "Consumer items completed"
  Savant8      = 100,   ///< "Structure kit manufactured"
  Savant9      = 101,   ///< "Spider ready"
  Savant10     = 102,   ///< "Scorpion ready"
  Savant11     = 103,   ///< "Structure destroyed"
  Savnt206     = 104,   ///< "Vehicle destroyed"
  Savant13     = 105,   ///< "Database reconstructed"
  Savant14     = 106,   ///< "Enemy unit sighted"
  Savant15     = 107,   ///< "Vehicle repaired"
  Savnt204     = 108,   ///< "Structure repaired"
  Savant17     = 109,   ///< "Warning, missile launch detected"
  Savant18     = 110,   ///< "Warning, incomming missile"
  Savant19     = 111,   ///< "Morale is excellent"
  Savant20     = 112,   ///< "Morale is good"
  Savant21     = 113,   ///< "Morale is fair"
  Savant22     = 114,   ///< "Morale is poor"
  Savant23     = 115,   ///< "Morale is terrible"
  Savant24     = 116,   ///< "Food production in surplus"
  Savant25     = 117,   ///< "Caution, food storage diminishing"
  Savant26     = 118,   ///< "Warning, people are starving"
  Savant27     = 119,   ///< "Power levels optimal"
  Savant28     = 120,   ///< "Warning, power levels marginal"
  Savant29     = 121,   ///< "Warning, power shortage"
  Savant30     = 122,   ///< "New mission objective"
  Savant32     = 123,   ///< "Edward satellite deployed"
  Savant33     = 124,   ///< "Solar power satellite deployed"
  Savant46     = 125,   ///< "Rocket launched"
  Savant47     = 126,   ///< "RLV launched"
  Savant48     = 127,   ///< "RLV ready"
  Savant49     = 128,   ///< "Rocket ready"
  Savnt221     = 129,   ///< "Common metals storage needed"
  Savant51     = 130,   ///< "Rare metal storage needed"
  Savant53     = 131,   ///< "New worker ready"
  Savant58     = 132,   ///< "Our people are dying"
  Savnt202     = 133,   ///< "Structure disabled"
  Savnt227     = 134,   ///< (Beep) "You have failed, our colony is doomed"
  Savant65     = 135,   ///< (Beep) "You have done well, our colony is surviving"
  Savant66     = 136,   ///< "Caution, volcanoe watch initiated"
  Savant67     = 137,   ///< "Alert, volcanic eruption imminent"
  Savant68     = 138,   ///< "Warning, volcanic eruption detected"
  Savant69     = 139,   ///< "Caution, vortex watch initiated"
  Savant70     = 140,   ///< "Alert, vortex imminent"
  Savant71     = 141,   ///< "Warning, vortex detected"
  Savant72     = 142,   ///< "Caution, electrical storm watch initiated"
  Savant73     = 143,   ///< "Alert, electrical storm imminent"
  Savant74     = 144,   ///< "Warning, electrical storm detected"
  Savant75     = 145,   ///< "Caution, meteor watch initiated"
  Savant76     = 146,   ///< "Alert, meteor impact imminent"
  Savant77     = 147,   ///< "Warning, meteor approaching"
  Savant78     = 148,   ///< "Caution, seismic watch initiated"
  Savant79     = 149,   ///< "Alert, seismic event imminent"
  Savant80     = 150,   ///< "Warning, seismic event detected"
  Savant93     = 151,   ///< "Wreckage located"
  Savnt278     = 152,   ///< "The blight is approching"
  Savnt207     = 153,   ///< "Damage warning"
  Savant96     = 154,   ///< "Mining location surveyed"
  Savant97     = 155,   ///< "Cannot deploy"
  Savant98     = 156,   ///< "Command control initiated"
  Savant99     = 157,   ///< "Savant series computer activated.  Outpost2 demo initiated.  Please choose an option."
  Savnt100     = 158,   ///< "Can not comply"
  /// "Commander, a volcanic eruption is imminent.  Evacuate the colony and rendezvous at the mining beacon north of our
  ///  location."
  Savnt101     = 159,   
  Savnt201     = 160,   ///< "Data transmitted"
  Savnt203     = 161,   ///< "Structures disabled"
  Savnt205     = 162,   ///< "Reinforcements ready"
  Savnt208     = 163,   ///< "Wall completed"
  Savnt209     = 164,   ///< "Tube completed"
  Savnt210     = 165,   ///< "Bulldozing completed"
  Savnt211     = 166,   ///< "Vehicle infected"
  Savnt213     = 167,   ///< "Structure lost"
  Savnt214     = 168,   ///< "Vehicle lost"
  Savnt215     = 169,   ///< "Vehicle captured"
  Savnt216     = 170,   ///< "Repair stopped"
  Savnt218     = 171,   ///< "Outpost2 demo mode initiated"
  Savnt219     = 172,   ///< "Please stand by"
  Savnt220     = 173,   ///< "Command control terminated"
  Savnt223     = 174,   ///< "Starship module manufactured"
  Savnt224     = 175,   ///< "Satellite manufactured"
  Savnt225     = 176,   ///< "Wreckage loaded"
  Savnt226     = 177,   ///< "Food stores are plentiful"
  Savnt228     = 178,   ///< "Alliance formed with player 1"
  Savnt229     = 179,   ///< "Alliance formed with player 2"
  Savnt230     = 180,   ///< "Alliance formed with player 3"
  Savnt231     = 181,   ///< "Alliance formed with player 4"
  Savnt232     = 182,   ///< "Alliance formed with player 5"
  Savnt233     = 183,   ///< "Alliance formed with player 6"
  Savnt234     = 184,   ///< "Alliance with player 1 broken"
  Savnt235     = 185,   ///< "Alliance with player 2 broken"
  Savnt236     = 186,   ///< "Alliance with player 3 broken"
  Savnt237     = 187,   ///< "Alliance with player 4 broken"
  Savnt238     = 188,   ///< "Alliance with player 5 broken"
  Savnt239     = 189,   ///< "Alliance with player 6 broken"
  Savnt240     = 190,   ///< "Transfer to player 1 complete"
  Savnt241     = 191,   ///< "Transfer to player 2 complete"
  Savnt242     = 192,   ///< "Transfer to player 3 complete"
  Savnt243     = 193,   ///< "Transfer to player 4 complete"
  Savnt244     = 194,   ///< "Transfer to player 5 complete"
  Savnt245     = 195,   ///< "Transfer to player 6 complete"
  Savnt246     = 196,   ///< "Transfer received from player 1"
  Savnt247     = 197,   ///< "Transfer received from player 2"
  Savnt248     = 198,   ///< "Transfer received from player 3"
  Savnt249     = 199,   ///< "Transfer received from player 4"
  Savnt250     = 200,   ///< "Transfer received from player 5"
  Savnt251     = 201,   ///< "Transfer received from player 6"
  Savnt252     = 202,   ///< "Player 1 has allied with you"
  Savnt253     = 203,   ///< "Player 2 has allied with you"
  Savnt254     = 204,   ///< "Player 3 has allied with you"
  Savnt255     = 205,   ///< "Player 4 has allied with you"
  Savnt256     = 206,   ///< "Player 5 has allied with you"
  Savnt257     = 207,   ///< "Player 6 has allied with you"
  Savnt258     = 208,   ///< "Player 1 has broken the alliance"
  Savnt259     = 209,   ///< "Player 2 has broken the alliance"
  Savnt260     = 210,   ///< "Player 3 has broken the alliance"
  Savnt261     = 211,   ///< "Player 4 has broken the alliance"
  Savnt262     = 212,   ///< "Player 5 has broken the alliance"
  Savnt263     = 213,   ///< "Player 6 has broken the alliance"
  Savnt264     = 214,   ///< "Ion drive module deployed"
  Savnt265     = 215,   ///< "Fusion drive module deployed"
  Savnt266     = 216,   ///< "Command module deployed"
  Savnt267     = 217,   ///< "Fueling systems deployed"
  Savnt268     = 218,   ///< "Habitat ring deployed"
  Savnt269     = 219,   ///< "Sensor package deployed"
  Savnt270     = 220,   ///< "Skydock operational"
  Savnt271     = 221,   ///< "Statis systems deployed"
  Savnt272     = 222,   ///< "Orbital package deployed"
  Savnt273     = 223,   ///< "Pheonix module deployed"
  Savnt274     = 224,   ///< "Rare metals storage module deployed"
  Savnt275     = 225,   ///< "Common metals storage module deployed"
  Savnt276     = 226,   ///< "Food storage module deployed"
  Savnt277     = 227,   ///< "Colonist evacuation module deployed"
  SavantEnd    = 227,

  Mis_fire     = 228,
  Acid2        = 229,
  //  Mis_fire = 230,
  //  Geo_sel  = 231,
  Laser        = 232,
  //  Medexp1  = 233,
  Microwve     = 234,
  //  Medexp1  = 235,
  Railfir      = 236,
  Railtar      = 237,
  //  Bigexp1  = 238,
  //  Bigexp2  = 239,
  //  Mis_fire = 240,
  Spam1        = 241,
  //  Mis_fire = 242,
  Foam         = 243,
  Thor         = 244,
  //  Medexp1  = 245,
  //  Mis_fire = 246,
  //  Medexp2  = 247,
  //  Laser    = 248,
  //  Smalxp1  = 249,
  Gun1         = 250,
  //  Medexp1  = 251,
  Count        = 252,
};

enum class SongID : int {
  Eden11 = 0,  // 0x0
  Eden21,      // 0x1
  Eden22,      // 0x2
  Eden31,      // 0x3
  Eden32,      // 0x4
  Eden33,      // 0x5
  EP41,        // 0x6
  EP42,        // 0x7
  EP43,        // 0x8
  EP51,        // 0x9
  EP52,        // 0xA
  EP61,        // 0xB
  EP62,        // 0xC
  EP63,        // 0xD
  Plymth11,    // 0xE
  Plymth12,    // 0xF
  Plymth21,    // 0x10
  Plymth22,    // 0x11
  Plymth31,    // 0x12
  Plymth32,    // 0x13
  Plymth33,    // 0x14
  Static01,    // 0x15
  Static02,    // 0x16
  Static03,    // 0x17
  Static04,    // 0x18
  Static05,    // 0x19
  Count
};

namespace TethysAPI {
  using SoundID = Tethys::SoundID;
  using SongID  = Tethys::SongID;
} // TethysAPI

} // Tethys
