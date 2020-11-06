/*
	Copyright 2016-2017 sub1to

	This file is part of subVersion GTA:O SC External Hack.

	subVersion GTA:O SC External Hack is free software: you can redistribute
	it and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	subVersion GTA:O SC External Hack is distributed in the hope that it
	will be useful, but WITHOUT ANY WARRANTY; without even the implied
	warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
	the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with subVersion GTA:O SC External Hack.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"

//global vars
HWND		g_hWnd;
hack* g_pHack;
settings* g_pSettings;
memManager* g_pMemMan;
D3D9Render* g_pD3D9Render;
std::map<int, CallbackProxy<hack, float>*>* g_pCBMap;
int			g_iFeature[MAX_MENU_FEATURES] = {};
int			g_iIndex;
int			g_iFeaturePlayerList[32];
std::pair<int, std::wstring> tbl_SessionMItems[] = {
	{-1,L"Ke GTA offline"},{0,L"Join public"},{1,L"Bikin Solo Public"},{12,L"Join temen"},
	{2,L"Bikin friend only"},{3,L"Gang war?"},{9,L"Add sbg teman"},{6,L"Close friend"},
	{10,L"Solo"},{11,L"Sesi invite only"}
};

bool		g_bKillSwitch = false;
bool		g_bKillRender = false;
bool		g_bKillAttach = false;
bool		g_bKillHack = false;

long		ADDRESS_WORLD = 0;
long		ADDRESS_BLIP = 0;
long		ADDRESS_AMMO = 0;
long		ADDRESS_MAGAZINE = 0;
long		ADDRESS_TUNABLE = 0;
long		ADDRESS_WEAPON = 0;
long		ADDRESS_GLOBAL = 0;
long		ADDRESS_PLAYER_LIST = 0;
long		ADDRESS_REPLAY_INTERFACE = 0;
//fuction prototypes
LRESULT	__stdcall	WindowProc(HWND	hWnd,
	UINT	message,
	WPARAM	wParam,
	LPARAM	lParam);
int					addFeature(int cat,
	int parent,
	std::wstring name,
	featType type,
	void (hack::* fun)(float*),
	float arg);
DWORD __stdcall		threadAttach(LPVOID lpParam);
DWORD __stdcall		threadRender(LPVOID lpParam);
DWORD __stdcall		threadHack(LPVOID lpParam);

void iklan();

int __stdcall WinMain(HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPSTR		lpCmdLine,
	int			nCmdShow)
{
	if (FindWindow("sub1toOverlay", nullptr))	//make sure the hack is not already running
		exit(0);

	g_iIndex = 0;
	g_pMemMan = new memManager;
	g_pSettings = new settings;
	g_pD3D9Render = new D3D9Render;
	g_pHack = new hack;
	g_pCBMap = new std::map<int, CallbackProxy<hack, float>*>;

	LPCSTR	szWindowTitleTarget = "Grand Theft Auto V";
	LPCWSTR	szWindowTitle = L"Wahyu anekdot mk2";
	//v 1.3.1
	g_pMemMan->setWindowName(szWindowTitleTarget);
	g_pD3D9Render->m_szWindowTitle = szWindowTitle;

	g_pSettings->addFeatureCategory(L"Pemain");		//0
	g_pSettings->addFeatureCategory(L"Senjata");		//1
	g_pSettings->addFeatureCategory(L"Mobil");		//2
	g_pSettings->addFeatureCategory(L"Teleport");		//3
	g_pSettings->addFeatureCategory(L"dll");		//4


	g_iFeature[FEATURE_P_TRUEGOD] = g_pSettings->addFeature(0, -1, L"Tuhan", feat_toggle, "trueGodMode");
	g_iFeature[FEATURE_P_GOD] = g_pSettings->addFeature(0, -1, L"Dewa", feat_toggle, "godMode");
	g_iFeature[FEATURE_P_HEAL] = addFeature(0, -1, L"ngobat", feat_btn, &hack::healPlayer, -1.f);
	g_iFeature[FEATURE_P_SUICIDE] = addFeature(0, -1, L"Bunuh diri", feat_btn, &hack::suicide, -1.f);
	g_iFeature[FEATURE_P_WANTED] = g_pSettings->addFeature(0, -1, L"Wanted", feat_slider, "wanted", 0.f, 5.f, .2f);
	g_iFeature[FEATURE_P_NEVERWANTED] = g_pSettings->addFeature(0, -1, L"Polisi Damai", feat_toggle, "neverWanted");
	//addFeature(0, -1, "杀死所有NPC", feat_btn, &hack::killAllNpc, -1.f);
	g_iFeature[FEATURE_P_ANTINPC] = g_pSettings->addFeature(0, -1, L"bunuh NPC", feat_toggle, "antiNpc");
	g_iFeature[FEATURE_P_NPC_IGNORE] = g_pSettings->addFeature(0, -1, L"NPC cuek", feat_toggle, "npcIgnore");
	g_iFeature[FEATURE_P_RUNSPD] = g_pSettings->addFeature(0, -1, L"kecepatan Lari", feat_slider, "runSpd", 1.f, 5.f);
	g_iFeature[FEATURE_P_SWIMSPD] = g_pSettings->addFeature(0, -1, L"kecepatan Renang", feat_slider, "swimSpd", 1.f, 5.f);
	g_iFeature[FEATURE_P_SUPER_PUNCH] = g_pSettings->addFeature(0, -1, L"Tinju Budha", feat_slider, "superPunch", 0.f, 1000.f, (float)1.f / 10.f);
	g_iFeature[FEATURE_P_SUPERJUMP] = g_pSettings->addFeature(0, -1, L"Lompat Kanguru", feat_toggle, "superJump");
	g_iFeature[FEATURE_P_EXPLOSIVEMELEE] = g_pSettings->addFeature(0, -1, L"Tinju Ledak Budha", feat_toggle, "explMelee");
	g_iFeature[FEATURE_P_UNDEAD_OFFRADAR] = g_pSettings->addFeature(0, -1, L"anti RadarUndead", feat_toggle, "undeadOffradar");
	g_iFeature[FEATURE_P_NORAGDOLL] = g_pSettings->addFeature(0, -1, L"anti Ragdoll", feat_toggle, "noRagdoll");
	g_iFeature[FEATURE_P_WATER_PROOF] = g_pSettings->addFeature(0, -1, L"anti Aer", feat_toggle, "waterProof");
	g_iFeature[FEATURE_P_STAMINA] = g_pSettings->addFeature(0, -1, L"Stamina Dewa", feat_toggle, "infStam");

	//g_iFeature[FEATURE_W_FILL_ALL_AMMO]		= addFeature(1, -1, "补满所有武器弹药", feat_btn, &hack::fillAllAmmo, -1.f);
	g_iFeature[FEATURE_W_FILL_AMMO] = addFeature(1, -1, L"IsiPelor", feat_btn, &hack::fillAmmo, -1.f);
	g_iFeature[FEATURE_W_SPREAD] = g_pSettings->addFeature(1, -1, L"No Nyebar", feat_toggle, "noSpread");
	g_iFeature[FEATURE_W_RECOIL] = g_pSettings->addFeature(1, -1, L"No Recoil", feat_toggle, "noRecoil");
	g_iFeature[FEATURE_W_NORELOAD] = g_pSettings->addFeature(1, -1, L"No Isi Ulang", feat_toggle, "noReload");
	g_iFeature[FEATURE_W_RELOAD] = g_pSettings->addFeature(1, -1, L"Isi Ulang GC", feat_slider, "quickReload", 1.f, 10.f);
	int bulletEdit = g_pSettings->addFeature(1, -1, L"Edit Peluru >>", feat_parent);
	g_iFeature[FEATURE_W_BULLET_EDIT] = g_pSettings->addFeature(-1, bulletEdit, L"Edit Peluru", feat_toggle, "bulletEdit");
	addFeature(-1, bulletEdit, L"Ledakan Granat", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::GrenadeExplosion);
	addFeature(-1, bulletEdit, L"Ledakan Sticky", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::StickyBombExplosion);
	addFeature(-1, bulletEdit, L"Ledakan Molotov", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::MoltovCoctailExplosion);
	addFeature(-1, bulletEdit, L"Super Launcher", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::SuperLauncher);
	addFeature(-1, bulletEdit, L"Ledakan petir", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::LightningExplosion);
	addFeature(-1, bulletEdit, L"Ledakan besar", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::BigExplosion);
	addFeature(-1, bulletEdit, L"Ledakan berapi", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::BigFireyExplosion);
	addFeature(-1, bulletEdit, L"Ledakan sedang", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::MediumExplosion);
	addFeature(-1, bulletEdit, L"Ledakan kecil", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::TinyExplosion);
	addFeature(-1, bulletEdit, L"Ledakan kecil 2", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::TinyExplosions2);
	addFeature(-1, bulletEdit, L"Semprot air kecil", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::SmallWaterSpray);
	addFeature(-1, bulletEdit, L"Semprot api kecil", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::SmallFireSpray);
	addFeature(-1, bulletEdit, L"Semprot air besar", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::BigWaterSpray);
	addFeature(-1, bulletEdit, L"Semprot api besar", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::BigFireSpray);
	addFeature(-1, bulletEdit, L"Peluru ledak MK2", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::MK2ExplosiveBullets);
	addFeature(-1, bulletEdit, L"Granat asap", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::SmokeGrenade);
	addFeature(-1, bulletEdit, L"Gas air mata", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::TearGas);
	addFeature(-1, bulletEdit, L"Gas air mata 2", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::TearGas2);
	addFeature(-1, bulletEdit, L"Flare merah", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::RedFlareSmoke);
	addFeature(-1, bulletEdit, L"Cool Ground Explosion", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::CoolGroundExplosion);
	addFeature(-1, bulletEdit, L"Ledakan shockwave", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::CRAZYSHOCKWAVEEXPLOSION);
	addFeature(-1, bulletEdit, L"Ledakan berapi besar", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::HUGEFireyExplosion);
	addFeature(-1, bulletEdit, L"Ledakan blimp besar", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::MassiveBlimpExplosion);
	addFeature(-1, bulletEdit, L"Ledakan blimp besar 2", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::MassiveBlimpExplosion2);
	addFeature(-1, bulletEdit, L"Serpihan ledak", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::LargeExplosionFallingDebris);
	addFeature(-1, bulletEdit, L"Ledakan bola api", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::FireBallExplosion);
	addFeature(-1, bulletEdit, L"Ledakan petasan", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::FireworkExplosion);
	addFeature(-1, bulletEdit, L"Bola salju", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::SnowballHit);
	addFeature(-1, bulletEdit, L"Layar gempa", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::JustScreenShake);
	addFeature(-1, bulletEdit, L"Spoof Exp", feat_btn, &hack::setImpactExplosion, ImpactExplosionEnum::SPOOFEXPLOSION);
	g_iFeature[FEATURE_W_FORCE_ON_PED] = g_pSettings->addFeature(1, -1, L"Gaya pejalan kaki", feat_slider, "forceOnPed", 1.f, 10000.f);
	g_iFeature[FEATURE_W_FORCE_ON_VEHICLE] = g_pSettings->addFeature(1, -1, L"Gaya pada kendaraan", feat_slider, "forceOnVeh", 1.f, 10000.f);
	g_iFeature[FEATURE_W_FORCE_ON_HELI] = g_pSettings->addFeature(1, -1, L"Gaya pada heli", feat_slider, "forceOnHeli", 1.f, 10000.f);
	g_iFeature[FEATURE_W_DAMAGE] = g_pSettings->addFeature(1, -1, L"Damage peluru", feat_slider, "bulletDamage", 1.f, 10.f);
	g_iFeature[FEATURE_W_AMMO] = g_pSettings->addFeature(1, -1, L"Peluru ga habis", feat_toggle, "infAmmo");
	g_iFeature[FEATURE_W_RANGE] = g_pSettings->addFeature(1, -1, L"Weap Range", feat_slider, "weapRange", 1.f, 10.f);
	g_iFeature[FEATURE_W_SPINUP] = g_pSettings->addFeature(1, -1, L"Weap spin", feat_toggle, "weapSpin");
	g_iFeature[FEATURE_W_EXPLOSIVEAMMO] = g_pSettings->addFeature(1, -1, L"Peluru Ledak", feat_toggle, "explAmmo");
	g_iFeature[FEATURE_W_FIREAMMO] = g_pSettings->addFeature(1, -1, L"Peluru api", feat_toggle, "fireAmmo");
	g_iFeature[FEATURE_W_BULLET_BATCH] = g_pSettings->addFeature(1, -1, L"Peluru banyak", feat_slider, "bulletBatch", 1.f, 10.f, (float)1.f / 9.f);
	g_iFeature[FEATURE_W_MUZZLE_VELOCITY] = g_pSettings->addFeature(1, -1, L"Kelajuan peluru", feat_slider, "muzzleVelo", 1.f, 10.f);

	g_iFeature[FEATURE_V_TRUEGOD] = g_pSettings->addFeature(2, -1, L"Kendaraan Tuhan", feat_toggle, "vehTrueGodMode");
	g_iFeature[FEATURE_V_GOD] = g_pSettings->addFeature(2, -1, L"Kendaraan Dewa", feat_toggle, "vehGodMode");
	g_iFeature[FEATURE_V_HEAL] = addFeature(2, -1, L"Ketok magic", feat_btn, &hack::healVehicle, -1.f);
	g_iFeature[FEATURE_V_BULLETPROOFTIRES] = g_pSettings->addFeature(2, -1, L"Ban antipeluru", feat_toggle, "vehBulletproofTires");
	g_iFeature[FEATURE_V_SEATBELT] = g_pSettings->addFeature(2, -1, L"Sabuk pengaman", feat_toggle, "seatbelt");
	g_iFeature[FEATURE_V_GRAVITY] = g_pSettings->addFeature(2, -1, L"Gravitasi Kendaraan", feat_slider, "vehGravity", -25.f, 25.f);
	g_iFeature[FEATURE_V_BOOST] = g_pSettings->addFeature(2, -1, L"Boost Kendaraan", feat_toggle, "vehBoost");
	g_iFeature[FEATURE_V_RECHARGE_SPEED] = g_pSettings->addFeature(2, -1, L"Boost isi ulang", feat_slider, "vehRrchargeSpeed", .5f, 5.f);
	int handing = g_pSettings->addFeature(2, -1, L"Setingan mobil >>", feat_parent);
	g_iFeature[FEATURE_V_MASS] = g_pSettings->addFeature(-1, handing, L"Massa", feat_slider, "vehMass", 0.f, 1000000.f);
	g_iFeature[FEATURE_V_BUOYANCY] = g_pSettings->addFeature(-1, handing, L"Kemampuan ngapung", feat_slider, "vehBuoyancy", 0.f, 1000.f);
	g_iFeature[FEATURE_V_ACCELERATION] = g_pSettings->addFeature(-1, handing, L"Akselerasi", feat_slider, "vehAccel", 1.f, 1000.f);
	g_iFeature[FEATURE_V_UPSHIFT] = g_pSettings->addFeature(-1, handing, L"Gigi naik", feat_slider, "vehUpShift", 1.f, 25.f);
	g_iFeature[FEATURE_V_DOWNSHIFT] = g_pSettings->addFeature(-1, handing, L"Gigi turun", feat_slider, "vehDownShift", 1.f, 25.f);
	g_iFeature[FEATURE_V_BRAKEFORCE] = g_pSettings->addFeature(-1, handing, L"Gaya rem", feat_slider, "vehBrakeForce", 1.f, 25.f);
	g_iFeature[FEATURE_V_HANDBRAKEFORCE] = g_pSettings->addFeature(-1, handing, L"Buyoancy", feat_slider, "vehBuoyancy", 1.f, 25.f);
	g_iFeature[FEATURE_V_TRACTION] = g_pSettings->addFeature(-1, handing, L"Traction", feat_slider, "vehTraction", 1.f, 25.f);
	g_iFeature[FEATURE_V_SUSPENSION_FORCE] = g_pSettings->addFeature(-1, handing, L"Gaya pegas", feat_slider, "vehSuspensionForce", 0.f, 25.f);
	g_iFeature[FEATURE_V_SUSPENSION_HEIGH] = g_pSettings->addFeature(-1, handing, L"Tinggi pegas", feat_slider, "vehSuspensionHeigh", 0.f, 1.f);
	g_iFeature[FEATURE_V_COLISION_DAMAGE_MP] = g_pSettings->addFeature(-1, handing, L"Kerusakan tabrakan", feat_slider, "vehColisionDamage", 0.f, 25.f);
	g_iFeature[FEATURE_V_WEAPON_DAMAGE_MP] = g_pSettings->addFeature(-1, handing, L"Damage senjata kendaraan", feat_slider, "vehWeaponDamage", 0.f, 25.f);
	g_iFeature[FEATURE_V_DEFORMATION] = g_pSettings->addFeature(-1, handing, L"Deform kendaraan", feat_slider, "vehDeform", 0.f, 25.f);
	g_iFeature[FEATURE_V_ENGINE_DAMAGE_MP] = g_pSettings->addFeature(-1, handing, L"Kerusakan mesin", feat_slider, "vehEngineDamage", 0.f, 25.f);

	g_pSettings->addFeature(3, -1, L"teleport ke WP", feat_teleport, tp_waypoint);
	g_pSettings->addFeature(3, -1, L"teleport ke OBJ", feat_teleport, tp_objective);
	addFeature(3, -1, L"Teleport maju", feat_btn, &hack::forwardTeleport, 5.f);

	int interior = g_pSettings->addFeature(3, -1, L"Teleport ke... >>", feat_parent);
	g_pSettings->addFeature(-1, interior, L"Gedung FIB", feat_teleport, tp_static, 136.0f, -750.f, 262.f);
	g_pSettings->addFeature(-1, interior, L"Garment Factory", feat_teleport, tp_static, 712.716f, -962.906f, 30.6f);
	g_pSettings->addFeature(-1, interior, L"Franklin House", feat_teleport, tp_static, 7.119f, 536.615f, 176.2f);
	g_pSettings->addFeature(-1, interior, L"Michael House", feat_teleport, tp_static, -813.603f, 179.474f, 72.5f);
	g_pSettings->addFeature(-1, interior, L"Hidden Treasure 1", feat_teleport, tp_static, 1972.610f, 3817.040f, 33.65f);
	g_pSettings->addFeature(-1, interior, L"Aunt Dennis House", feat_teleport, tp_static, -14.380f, -1438.510f, 31.3f);
	g_pSettings->addFeature(-1, interior, L"Freud House", feat_teleport, tp_static, -1151.770f, -1518.138f, 10.85f);
	g_pSettings->addFeature(-1, interior, L"Lester House", feat_teleport, tp_static, 1273.898f, -1719.304f, 54.8f);
	g_pSettings->addFeature(-1, interior, L"Strip Club", feat_teleport, tp_static, 97.271f, -1290.994f, 29.45f);
	g_pSettings->addFeature(-1, interior, L"Bank Vault (Pacific Standard)", feat_teleport, tp_static, 255.85f, 217.f, 101.9f);
	g_pSettings->addFeature(-1, interior, L"Comedy Club", feat_teleport, tp_static, 378.100f, -999.964f, -98.6f);
	g_pSettings->addFeature(-1, interior, L"Humane Lab", feat_teleport, tp_static, 3614.394f, 3744.803f, 28.9f);
	g_pSettings->addFeature(-1, interior, L"Humane Lab Authentic", feat_teleport, tp_static, 3525.201f, 3709.625f, 21.2f);
	g_pSettings->addFeature(-1, interior, L"IAA Office", feat_teleport, tp_static, 113.568f, -619.001f, 206.25f);
	g_pSettings->addFeature(-1, interior, L"Torture Chamber", feat_teleport, tp_static, 142.746f, -2201.189f, 4.9f);
	g_pSettings->addFeature(-1, interior, L"Military Base Tower", feat_teleport, tp_static, -2358.132f, 3249.754f, 101.65f);
	g_pSettings->addFeature(-1, interior, L"Mine", feat_teleport, tp_static, -595.342f, 2086.008f, 131.6f);
	int saved = g_pSettings->addFeature(3, -1, L"Custom Save Point >>", feat_parent);
	g_pSettings->addFeature(-1, saved, L"Save Point1", feat_teleport, "pos0", tp_saved);
	g_pSettings->addFeature(-1, saved, L"Save Point2", feat_teleport, "pos1", tp_saved);
	g_pSettings->addFeature(-1, saved, L"Save Point3", feat_teleport, "pos2", tp_saved);
	g_pSettings->addFeature(-1, saved, L"Save Point4", feat_teleport, "pos3", tp_saved);
	g_pSettings->addFeature(-1, saved, L"Save Point5", feat_teleport, "pos4", tp_saved);
	g_pSettings->addFeature(-1, saved, L"Save Point6", feat_teleport, "pos5", tp_saved);
	g_pSettings->addFeature(3, -1, L"Los Santos Custom", feat_teleport, tp_static, -365.425f, -131.809f, -225.f);//38.9f);
	g_pSettings->addFeature(3, -1, L"LS Airport", feat_teleport, tp_static, -1336.f, -3044.f, -225.f);//14.15f);
	g_pSettings->addFeature(3, -1, L"Shandy Shore Airport", feat_teleport, tp_static, 1747.f, 3273.f, -225.f);//41.35f);
	g_pSettings->addFeature(3, -1, L"Mount Chilliad", feat_teleport, tp_static, 489.979f, 5587.527f, 794.3f);

	int session = g_pSettings->addFeature(4, -1, L"Bikin sesi baru >>", feat_parent);
	for (size_t i = 0; i < sizeof(tbl_SessionMItems) / sizeof(tbl_SessionMItems[0]); i++)
		addFeature(-1, session, tbl_SessionMItems[i].second, feat_btn, &hack::loadSession, tbl_SessionMItems[i].first);
	int olService = g_pSettings->addFeature(4, -1, L"Onlen >>", feat_parent);
	addFeature(-1, olService, L"Ke PV", feat_btn, &hack::intoPV, -1.f);
	g_iFeature[FEATURE_P_MONERY_DROP] = g_pSettings->addFeature(-1, olService, L"Uang surga", feat_toggle, "moneyDrop");
	//g_iFeature[FEATURE_P_PLAYER_LIST] = g_pSettings->addFeature(3, -1, "玩家列表 >>", feat_parent);
	//for (size_t i = 0; i < sizeof(g_iFeaturePlayerList)/sizeof(g_iFeaturePlayerList[0]); i++)
	//{
	//	g_iFeaturePlayerList[i] = g_pSettings->addFeature(-1, g_iFeature[FEATURE_P_PLAYER_LIST],"线上 >>", feat_parent);
	//}
	int vehSpawn = g_pSettings->addFeature(-1, olService, L"Mobil >>", feat_parent);
	for (size_t i = 0; i < vehiclePreview.size(); i++)
	{
		int temp = g_pSettings->addFeature(-1, vehSpawn, vehiclePreview[i].first, feat_parent);
		for (size_t j = 0; j < vehiclePreview[i].second.size(); j++)
			addFeature(-1, temp, vehiclePreview[i].second[j].VName, feat_btn, &hack::spawnVehicle, i * 1000 + j);
	}

	int tunable = g_pSettings->addFeature(-1, olService, L"Parameter tunable >>", feat_parent);
	g_iFeature[FEATURE_T_ANTI_IDLE_KICK] = g_pSettings->addFeature(-1, tunable, L"Anti AFK", feat_toggle, "AntiIdleKick");
	g_iFeature[FEATURE_T_ORBITAL_CANNON] = g_pSettings->addFeature(-1, tunable, L"Orbital Cannon", feat_toggle, "OrbitalCannon");
	g_iFeature[FEATURE_T_RP_MP] = g_pSettings->addFeature(-1, tunable, L"RP Ganda", feat_slider, "RP", 1.f, 1000.f, (float)1.f / 9.f);
	g_iFeature[FEATURE_T_AP_MP] = g_pSettings->addFeature(-1, tunable, L"AP Ganda", feat_slider, "AP", 1.f, 1000.f, (float)1.f / 9.f);
	g_iFeature[FEATURE_T_MISSION_PAYOUT] = g_pSettings->addFeature(-1, tunable, L"Bayaran misi", feat_slider, "MinMissionPayout", 0.f, 100000.f);
	int recovery = g_pSettings->addFeature(-1, olService, L"Buka kunci >>", feat_parent);
	addFeature(-1, recovery, L"Isi Skill", feat_btn, &hack::fillSkillLevels, -1.f);
	addFeature(-1, recovery, L"Isi cemilan", feat_btn, &hack::fillAllSnacks, -1.f);
	int casino = g_pSettings->addFeature(-1, recovery, L"Perampokan Kasino >>", feat_parent);
	addFeature(-1, casino, L"Stat kasino", feat_btn, &hack::casinoStat, 1.f);
	int bitSet1 = g_pSettings->addFeature(-1, casino, L"Papan rencana >>", feat_parent);
	addFeature(-1, bitSet1, L"Unlock probe", feat_btn, &hack::casinoStatBitSet1, 7.f);
	addFeature(-1, bitSet1, L"Unlock POI", feat_btn, &hack::casinoStatBitSet1, 8.f);
	int approach = g_pSettings->addFeature(-1, bitSet1, L"抢劫方式 >>", feat_parent);
	addFeature(-1, approach, L"Stealth", feat_btn, &hack::casinoStatBitSet1, 0.f);
	addFeature(-1, approach, L"Big Cont", feat_btn, &hack::casinoStatBitSet1, 1.f);
	addFeature(-1, approach, L"Agressive", feat_btn, &hack::casinoStatBitSet1, 2.f);
	int target = g_pSettings->addFeature(-1, bitSet1, L"Rampok >>", feat_parent);
	addFeature(-1, target, L"Duid", feat_btn, &hack::casinoStatBitSet1, 3.f);
	addFeature(-1, target, L"Emas", feat_btn, &hack::casinoStatBitSet1, 4.f);
	addFeature(-1, target, L"Seni", feat_btn, &hack::casinoStatBitSet1, 5.f);
	addFeature(-1, target, L"Intan", feat_btn, &hack::casinoStatBitSet1, 6.f);
	int bitSet2 = g_pSettings->addFeature(-1, casino, L"Papan rencana kedua >>", feat_parent);
	addFeature(-1, bitSet2, L"One key, highest", feat_btn, &hack::casinoStatBitSet2, 11.f);
	addFeature(-1, bitSet2, L"Duggan Shipment kelar", feat_btn, &hack::casinoStatBitSet2, 0.f);
	addFeature(-1, bitSet2, L"Keycard level tinggi", feat_btn, &hack::casinoStatBitSet2, 1.f);
	addFeature(-1, bitSet2, L"Most sought-after", feat_btn, &hack::casinoStatBitSet2, 2.f);
	addFeature(-1, bitSet2, L"Driver terbaik", feat_btn, &hack::casinoStatBitSet2, 3.f);
	addFeature(-1, bitSet2, L"Hacker terbaik", feat_btn, &hack::casinoStatBitSet2, 4.f);
	int vehs = g_pSettings->addFeature(-1, bitSet2, L"Kendaraan escape >>", feat_parent);
	addFeature(-1, vehs, L"0", feat_btn, &hack::casinoStatBitSet2, 5.f);
	addFeature(-1, vehs, L"1", feat_btn, &hack::casinoStatBitSet2, 6.f);
	addFeature(-1, vehs, L"2", feat_btn, &hack::casinoStatBitSet2, 7.f);
	addFeature(-1, vehs, L"3", feat_btn, &hack::casinoStatBitSet2, 8.f);
	int weaps = g_pSettings->addFeature(-1, bitSet2, L"Senjata >>", feat_parent);
	addFeature(-1, weaps, L"0", feat_btn, &hack::casinoStatBitSet2, 9.f);
	addFeature(-1, weaps, L"1", feat_btn, &hack::casinoStatBitSet2, 10.f);
	int unlock = g_pSettings->addFeature(-1, recovery, L"Buka >>", feat_parent);
	addFeature(-1, unlock, L"Unlock LSC", feat_btn, &hack::unlockLSC, -1.f);
	addFeature(-1, unlock, L"Unlock Camo", feat_btn, &hack::unlockWeaponCamos, -1.f);
	addFeature(-1, unlock, L"Unlock Heist Car", feat_btn, &hack::unlockHeistCars, -1.f);
	addFeature(-1, unlock, L"Unlock Awards", feat_btn, &hack::unlockAllAwards, -1.f);
	addFeature(-1, unlock, L"Unlock Baju", feat_btn, &hack::unlockClothes, -1.f);
	g_iFeature[FEATURE_T_BUNKER_RESEARCH] = g_pSettings->addFeature(-1, unlock, L"Riset Bunker", feat_toggle, "BunkerResearch");
	addFeature(4, -1, L"Buka Internet", feat_btn, &hack::about, 0.f);
	//addFeature(4, -1, L"Cek Update", feat_btn, &hack::about, 1.f);
	addFeature(4, -1, L"Matiin modmenu", feat_btn, &hack::cabut, 1.f);


	g_pSettings->setActiveCat(0);			//this needs to be called so we can fill the current feature buffer

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hbrBackground = dynamic_cast<HBRUSH>(CreateSolidBrush(RGB(0, 0, 0)));//(HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "sub1toOverlay";

	RegisterClassEx(&wc);
	g_hWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,		//dwExStyle [in]
		L"sub1toOverlay",										//lpClassName [in, optional]
		szWindowTitle,											//lpWindowName [in, optional]
		WS_POPUP,												//dwStyle [in]
		0,														//x [in]
		0,														//y [in]
		300,													//nWidth [in]
		300,													//nHeight [in]
		nullptr,												//hWndParent [in, optional]
		nullptr,												//hMenu [in, optional]
		hInstance,												//hInstance [in, optional]		A handle to the instance of the module to be associated with the window.
		nullptr);												//lpParam [in, optional]

	SetLayeredWindowAttributes(g_hWnd, 0, 0, LWA_ALPHA);
	SetLayeredWindowAttributes(g_hWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

	MARGINS margins{ 1, 1, 1, 1 };
	DwmExtendFrameIntoClientArea(g_hWnd, &margins);

	ShowWindow(g_hWnd, SW_SHOWNORMAL);

	CreateThread(NULL,
		0,
		threadAttach,
		NULL,
		0,
		nullptr);
	Sleep(100);
	CreateThread(NULL,
		0,
		threadRender,
		NULL,
		0,
		nullptr);
	Sleep(100);
	CreateThread(NULL,
		0,
		threadHack,
		NULL,
		0,
		nullptr);

	MSG msg;
	while (true)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Check to see if any messages are waiting in the queue
		{
			TranslateMessage(&msg);		//Translate the message and dispatch it to WindowProc()
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		Sleep(100);
	}

	return (int)msg.wParam;
}

//main message handler
LRESULT __stdcall WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam); //default behaviour for any unhandled messages
}

int addFeature(int cat, int parent, std::wstring name, featType type, void (hack::* func)(float*), float arg)
{
	(*g_pCBMap)[g_iIndex] = new CallbackProxy<hack, float>;
	(*g_pCBMap)[g_iIndex]->Set(g_pHack, func);
	return g_pSettings->addFeature(cat, parent, name, type, g_iIndex++, arg);
}

void iklan() {
	 MessageBoxExW(
		NULL,
		L"Nyalain NUM LOCK dan tekan NUM-",
		L"Mod menu terbuka",
		MB_ICONWARNING | MB_OK | MB_DEFBUTTON2,
		0
	);
}

DWORD __stdcall threadAttach(LPVOID lpParam)
{	
	//iklan();

	while (!g_bKillSwitch)
	{
		if (g_pMemMan->attach() && g_pMemMan->findWindow())
		{

			HWND	fgWnd = GetForegroundWindow(),
				tgWnd = g_pMemMan->getWindow();
			if (g_pD3D9Render->getViewport())
				MoveWindow(g_hWnd, g_pD3D9Render->m_screen.x, g_pD3D9Render->m_screen.y, g_pD3D9Render->m_screen.w, g_pD3D9Render->m_screen.h, true);

			if (fgWnd != tgWnd && fgWnd != g_hWnd)
			{
				SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			else if (g_hWnd == fgWnd)
			{
				ShowWindow(tgWnd, SW_SHOW);
				SetForegroundWindow(tgWnd);
			}
			else if (tgWnd == fgWnd && !(GetWindowLong(g_hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST))
			{
				SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
		}
		else
		{
			MessageBoxW(nullptr, L"GTA V nya idupin dulu jir", L"Mod menu gagal kebukak", MB_OK | MB_ICONERROR);
			g_bKillAttach = true;
			killProgram();
		}

		Sleep(0x30);
	}
	g_bKillAttach = true;
	return 1;
}

DWORD __stdcall threadRender(LPVOID lpParam)
{
	g_pD3D9Render->init(g_hWnd);
	while (!g_bKillSwitch)
	{
		g_pD3D9Render->render();
		Sleep(0x10);
	}
	g_bKillRender = true;
	return 0;
}

DWORD __stdcall threadHack(LPVOID lpParam)
{
	g_pHack->m_hModule = g_pMemMan->getModuleAddress("GTA5.exe");
	while (!g_bKillSwitch)
	{
		BYTE btInit = g_pHack->initPointers();
		g_pHack->checkKeys();

		if (!(btInit & INITPTR_INVALID_WORLD) && !(btInit & INITPTR_INVALID_PLAYER))
		{
			if (g_pSettings->getFeature(g_iFeature[FEATURE_P_GOD])->m_bOn || g_pSettings->getFeature(g_iFeature[FEATURE_P_TRUEGOD])->m_bOn)
				g_pHack->restoreHealth();
			if (g_pSettings->getFeature(g_iFeature[FEATURE_P_ANTINPC])->m_bOn)
				g_pHack->killNpc();
			if (g_pSettings->getFeature(g_iFeature[FEATURE_P_NEVERWANTED])->m_bOn)
				g_pHack->notWanted();
			if (g_pSettings->getFeature(g_iFeature[FEATURE_P_STAMINA])->m_bOn)
				g_pHack->restoreStamina();

			g_pHack->neverWanted(g_pSettings->getFeature(g_iFeature[FEATURE_P_NEVERWANTED]));
			g_pHack->wanted(g_pSettings->getFeature(g_iFeature[FEATURE_P_WANTED]));
			g_pHack->runSpeed(g_pSettings->getFeature(g_iFeature[FEATURE_P_RUNSPD]));
			g_pHack->swimSpeed(g_pSettings->getFeature(g_iFeature[FEATURE_P_SWIMSPD]));
			g_pHack->npcIgnore(g_pSettings->getFeature(g_iFeature[FEATURE_P_NPC_IGNORE]));
			g_pHack->godMode(g_pSettings->getFeature(g_iFeature[FEATURE_P_TRUEGOD]));
			g_pHack->noRagdoll(g_pSettings->getFeature(g_iFeature[FEATURE_P_NORAGDOLL]));
			g_pHack->seatbelt(g_pSettings->getFeature(g_iFeature[FEATURE_V_SEATBELT]));
			g_pHack->superPunch(g_pSettings->getFeature(g_iFeature[FEATURE_P_SUPER_PUNCH]));
			if (g_pHack->m_player.loadPlayerData())
			{
				g_pHack->undeadOffradar(g_pSettings->getFeature(g_iFeature[FEATURE_P_UNDEAD_OFFRADAR]));
				g_pHack->waterProof(g_pSettings->getFeature(g_iFeature[FEATURE_P_WATER_PROOF]));
			}

			g_pHack->frameFlags(g_pSettings->getFeature(g_iFeature[FEATURE_P_SUPERJUMP]),
				g_pSettings->getFeature(g_iFeature[FEATURE_P_EXPLOSIVEMELEE]),
				g_pSettings->getFeature(g_iFeature[FEATURE_W_FIREAMMO]),
				g_pSettings->getFeature(g_iFeature[FEATURE_W_EXPLOSIVEAMMO]));


			if (!(btInit & INITPTR_INVALID_VEHICLE))
			{
				if (g_pSettings->getFeature(g_iFeature[FEATURE_V_GOD])->m_bOn)
					g_pHack->restoreVehicleHealth();

				g_pHack->vehicleGod(g_pSettings->getFeature(g_iFeature[FEATURE_V_TRUEGOD]));
				g_pHack->vehicleGravity(g_pSettings->getFeature(g_iFeature[FEATURE_V_GRAVITY]));
				g_pHack->vehicleBulletproofTires(g_pSettings->getFeature(g_iFeature[FEATURE_V_BULLETPROOFTIRES]));
				g_pHack->boost(g_pSettings->getFeature(g_iFeature[FEATURE_V_BOOST]));
				g_pHack->vehicleRocketRechargeSpeed(g_pSettings->getFeature(g_iFeature[FEATURE_V_RECHARGE_SPEED]));

				if (g_pHack->m_vehicle.loadHandling())
				{
					g_pHack->vehicleAccel(g_pSettings->getFeature(g_iFeature[FEATURE_V_ACCELERATION]));
					g_pHack->vehicleBrake(g_pSettings->getFeature(g_iFeature[FEATURE_V_BRAKEFORCE]));
					g_pHack->vehicleTraction(g_pSettings->getFeature(g_iFeature[FEATURE_V_TRACTION]));
					g_pHack->vehicleDeformation(g_pSettings->getFeature(g_iFeature[FEATURE_V_DEFORMATION]));
					g_pHack->vehicleUpShift(g_pSettings->getFeature(g_iFeature[FEATURE_V_UPSHIFT]));
					g_pHack->vehicleDownShift(g_pSettings->getFeature(g_iFeature[FEATURE_V_DOWNSHIFT]));
					g_pHack->vehicleSuspensionForce(g_pSettings->getFeature(g_iFeature[FEATURE_V_SUSPENSION_FORCE]));
					g_pHack->vehicleMass(g_pSettings->getFeature(g_iFeature[FEATURE_V_MASS]));
					g_pHack->vehicleBuoyancy(g_pSettings->getFeature(g_iFeature[FEATURE_V_BUOYANCY]));
					g_pHack->vehicleHandbrakeForce(g_pSettings->getFeature(g_iFeature[FEATURE_V_HANDBRAKEFORCE]));
					g_pHack->vehicleSuspensionHeigh(g_pSettings->getFeature(g_iFeature[FEATURE_V_SUSPENSION_HEIGH]));
					g_pHack->vehicleColisionDamageMult(g_pSettings->getFeature(g_iFeature[FEATURE_V_COLISION_DAMAGE_MP]));
					g_pHack->vehicleWeaponDamageMult(g_pSettings->getFeature(g_iFeature[FEATURE_V_WEAPON_DAMAGE_MP]));
					g_pHack->vehicleEngineDamageMult(g_pSettings->getFeature(g_iFeature[FEATURE_V_ENGINE_DAMAGE_MP]));
				}
			}

			if (!(btInit & INITPTR_INVALID_WEAPON) && g_pHack->m_weapon.loadWeapon())
			{
				g_pHack->weaponBulletEdit(g_pSettings->getFeature(g_iFeature[FEATURE_W_BULLET_EDIT]));
				g_pHack->noSpread(g_pSettings->getFeature(g_iFeature[FEATURE_W_SPREAD]));
				g_pHack->noRecoil(g_pSettings->getFeature(g_iFeature[FEATURE_W_RECOIL]));
				g_pHack->quickReload(g_pSettings->getFeature(g_iFeature[FEATURE_W_RELOAD]));
				g_pHack->bulletDamage(g_pSettings->getFeature(g_iFeature[FEATURE_W_DAMAGE]));
				g_pHack->weaponRange(g_pSettings->getFeature(g_iFeature[FEATURE_W_RANGE]));
				g_pHack->weaponSpin(g_pSettings->getFeature(g_iFeature[FEATURE_W_SPINUP]));
				g_pHack->weaponForceOnPed(g_pSettings->getFeature(g_iFeature[FEATURE_W_FORCE_ON_PED]));
				g_pHack->weaponForceOnVehicle(g_pSettings->getFeature(g_iFeature[FEATURE_W_FORCE_ON_VEHICLE]));
				g_pHack->weaponForceOnHeli(g_pSettings->getFeature(g_iFeature[FEATURE_W_FORCE_ON_HELI]));
				g_pHack->bulletBatch(g_pSettings->getFeature(g_iFeature[FEATURE_W_BULLET_BATCH]));
				g_pHack->muzzleVelocity(g_pSettings->getFeature(g_iFeature[FEATURE_W_MUZZLE_VELOCITY]));
				g_pHack->infAmmo(g_pSettings->getFeature(g_iFeature[FEATURE_W_AMMO]));
				g_pHack->noReload(g_pSettings->getFeature(g_iFeature[FEATURE_W_NORELOAD]));
			}

			if (!(btInit & INITPTR_INVALID_TUNABLE))
			{
				g_pHack->tunableRpMult(g_pSettings->getFeature(g_iFeature[FEATURE_T_RP_MP]));
				g_pHack->tunableApMult(g_pSettings->getFeature(g_iFeature[FEATURE_T_AP_MP]));
				g_pHack->tunableMissionPayout(g_pSettings->getFeature(g_iFeature[FEATURE_T_MISSION_PAYOUT]));
				g_pHack->tunableOrbitalCannonCooldown(g_pSettings->getFeature(g_iFeature[FEATURE_T_ORBITAL_CANNON]));
				g_pHack->tunableBunkerResearch(g_pSettings->getFeature(g_iFeature[FEATURE_T_BUNKER_RESEARCH]));
				g_pHack->tunableAntiIdleKick(g_pSettings->getFeature(g_iFeature[FEATURE_T_ANTI_IDLE_KICK]));
			}

			if (!(btInit & INITPTR_INVALID_GLOBAL))
			{
				g_pHack->selfDropMoney(g_pSettings->getFeature(g_iFeature[FEATURE_P_MONERY_DROP]));
				g_pHack->consumeStatQueue();
			}

			//g_pHack->renderPlayerList(g_iFeature[FEATURE_P_PLAYER_LIST], g_iFeaturePlayerList);
		}
		Sleep(1);
	}
	g_bKillHack = true;
	return 0;
}

//Kill the program; Prototype in stdafx.h
void	killProgram()
{
	g_bKillSwitch = true;				//enable thread killswitch
	g_pSettings->m_iniParser.write();	//save options

	//make sure we shut down all threads before deleting the objects
	while (!g_bKillAttach || !g_bKillRender || !g_bKillHack)
		Sleep(1);

	//restore patched code
	feat dummyFeat;
	dummyFeat.m_bRestored = false;
	g_pHack->noReload(&dummyFeat);
	dummyFeat.m_bRestored = false;
	g_pHack->infAmmo(&dummyFeat);

	delete	g_pHack;
	delete	g_pD3D9Render;
	delete	g_pSettings;
	delete	g_pMemMan;
	exit(0);
}

