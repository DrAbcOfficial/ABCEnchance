/*
* The plugin resumes sending deathmsg networkmessages to 
* achieve more precise death messages and death message localization
*/
class CDMGRecord{
	EHandle eInflictor;
	int iDamageType;
}
array<CDMGRecord> aryInflictor(33);
void PluginInit(){
	g_Module.ScriptInfo.SetAuthor( "Dr.Abc" );
	g_Module.ScriptInfo.SetContactInfo( "https://github.com/DrAbcOfficial/ABCEnchance" );
	g_Hooks.RegisterHook(Hooks::Player::PlayerTakeDamage, @PlayerTakeDamage);
	g_Hooks.RegisterHook(Hooks::Player::PlayerKilled, @PlayerKilled);
	g_Hooks.RegisterHook(Hooks::Player::ClientPutInServer, @ClientPutInServer);
}
void MapInit(){
	aryInflictor = array<CDMGRecord>(33);
}
HookReturnCode PlayerTakeDamage(DamageInfo@ info){
    if(@info.pVictim !is null && @info.pInflictor !is null){
        aryInflictor[info.pVictim.entindex()].eInflictor = EHandle(@info.pInflictor);
        aryInflictor[info.pVictim.entindex()].iDamageType = info.bitsDamageType;
    }
    return HOOK_CONTINUE;
}
HookReturnCode PlayerKilled(CBasePlayer@ pPlayer, CBaseEntity@ pAttacker, int iGib){
    if(pPlayer is null || !pPlayer.IsNetClient() || !pPlayer.IsConnected())
        return HOOK_CONTINUE;

    CBaseEntity@ pInflictor = aryInflictor[pPlayer.entindex()].eInflictor.GetEntity();
    if(pInflictor is null)
        @pInflictor = @pAttacker;
    for (int i = 0; i <= g_Engine.maxClients; i++){
        CBasePlayer@ ePlayer = g_PlayerFuncs.FindPlayerByIndex(i);
        if(ePlayer !is null && ePlayer.IsConnected()){
        	NetworkMessage message( MSG_ONE_UNRELIABLE, NetworkMessages::DeathMsg, ePlayer.edict());
        	//Type
			message.WriteByte((@ePlayer is @pPlayer || @ePlayer is @pAttacker) ? 2 : 1);
			//Who
			message.WriteShort(pAttacker.entindex());
			//Name
			string szName = pAttacker.pev.classname;
			if(@pAttacker is @pPlayer)
				szName = "";
			else if(pAttacker.IsMonster())
				szName = cast<CBaseMonster@>(@pAttacker).m_FormattedName;
			else if(pAttacker.IsPlayer())
			    szName = pAttacker.pev.netname;
			message.WriteString(szName);
			//With what
			if(@pAttacker is @pPlayer)
				szName = "Suicide";
			else if(@pAttacker is @pInflictor){
				if(pAttacker.IsPlayer()){
					szName = cast<CBasePlayer@>(@pAttacker).m_hActiveItem.GetEntity().pev.classname;
					szName.Trim("weapon_");
					szName[0] = toupper(szName[0]);
				}
				else
					szName = "Kill";
			}
			else
				szName = pInflictor.pev.classname;
			message.WriteString(szName);
			//Who died?
			message.WriteString(pPlayer.pev.netname);
			//DMG type
			message.WriteLong(aryInflictor[pPlayer.entindex()].iDamageType);
			message.End();
        }
    }
    return HOOK_CONTINUE;
}
HookReturnCode ClientPutInServer( CBasePlayer@ pPlayer ){
    NetworkMessage message( MSG_ONE_UNRELIABLE, NetworkMessages::DeathMsg, pPlayer.edict());
		message.WriteByte(0);
	message.End();
	return HOOK_CONTINUE;
}