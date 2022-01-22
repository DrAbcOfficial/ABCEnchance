
namespace EccoAddon{
namespace EccoABCEnchance{
    class CExtraItemInfo{
        string ScriptName;
        int Seqence;
        string ModelPath;
        int ModelIndex;
        CExtraItemInfo(string _s, int _r, string _p){
            ScriptName = _s;
            Seqence = _r;
            ModelPath = _p;
        }
        bool opEquals(string sname){
            return ScriptName == sname;
        }
    }
    enum Seqence{
        INVALID = -4,
        NONE = -3,
        ITEM = -2,
        PISTOL = 56,
        RIFLE = 72,
        GRENADE = 42,
        KNIFE = 20,
        HEAVY = 86
    }
    enum MenuToken{
        MENU_INVALID = -1,
        MENU_BACKUP = -2,
        MENU_NEXTPAGE = -3,
        MENU_LASTPAGE = -4
    }
    int MetaHookChannel = 146;
    int EccoInfoType = 7;
    int EccoMenuType = 8;

    array<CExtraItemInfo@> aryExtraInfos = {
        CExtraItemInfo("A", ITEM, "models/w_battery.mdl"),
        CExtraItemInfo("B", RIFLE, "models/p_m16.mdl"),
        CExtraItemInfo("C", GRENADE, "models/p_satchel.mdl"),
        CExtraItemInfo("D", PISTOL, "models/p_357.mdl"),
        CExtraItemInfo("E", KNIFE, "models/p_pipe_wrench.mdl"),
        CExtraItemInfo("F", HEAVY, "models/p_rpg.mdl"),
        CExtraItemInfo("G", ITEM, "models/w_longjump.mdl")
    };
    void PluginInit(){
        g_Hooks.RegisterHook(Hooks::Player::ClientPutInServer, @ClientPutInServer);
        EccoHook::RegisterHook(EccoHook::Economy::OpenBuyMenu, @OpenBuyMenu);
    }

    void MapInit(){
        for(uint i = 0; i < aryExtraInfos.length(); i++){
            aryExtraInfos[i].ModelIndex = g_Game.PrecacheModel(aryExtraInfos[i].ModelPath);
            g_Game.PrecacheGeneric(aryExtraInfos[i].ModelPath);

        }
    }
    string GetAuthor(){
      return "Dr.Abc";
    }
    string GetContactInfo(){
        return "100%私货";
    }
    /*
        3 EccoInfo
        0 | byte | msgtype
        4 | int | Id
        8 | int | Price
        12 | int | Model index
        16 | int | Seqence
        20 | string | Name
    */
    HookReturnCode ClientPutInServer(CBasePlayer@ pPlayer){
        NetworkMessage message(MSG_ONE, NetworkMessages::NetworkMessageType(39), pPlayer.edict()); // svc_newusermsg
            message.WriteByte(MetaHookChannel); // 64 ~ 145 = SelAmmo ~ VModelPos, all of them are reserved or used by Sven Co-op
            message.WriteByte(255); // 255 = variable length
            message.WriteLong(0x6174654D); // 'ateM'
            message.WriteLong(0x6B6F6F48); // 'kooH'
            message.WriteLong(0);
            message.WriteLong(0);
        message.End();

        for(uint i = 0; i < EccoBuyMenu::aryMenuItemList.length(); i++){
            CBaseMenuItem@ pIterator = EccoBuyMenu::aryMenuItemList[i];
            int index = -1;
            for(uint j = 0; j < aryExtraInfos.length(); j++){
                if(aryExtraInfos[j] == pIterator.ScriptName){
                    index = j;
                    break;
                }
            }
            NetworkMessage m(MSG_ONE, NetworkMessages::NetworkMessageType(MetaHookChannel), pPlayer.edict());
                m.WriteByte(EccoInfoType);
                m.WriteLong(pIterator.Id);
                m.WriteLong(pIterator.Cost);
                if(index >= 0){
                    m.WriteLong(aryExtraInfos[index].ModelIndex);
                    m.WriteLong(aryExtraInfos[index].Seqence);
                }
                else{
                    m.WriteLong(-1);
                    m.WriteLong(INVALID);
                }
                m.WriteString(pIterator.DisplayName);
            m.End();
        }
        return HOOK_CONTINUE;
    }

    /*
        4 EccoMenu
        0 | byte | msgtype
        4 | byte | Item count
        4 * (n + 1) | int | Id
    */
    HookReturnCode OpenBuyMenu(const int iDisplayTime, const uint iPage, CBasePlayer@ pPlayer, CBaseMenuItem@ pMenu){
        array<int> temp = {};
        for(uint i = 0; i < pMenu.length(); i++){
            temp.insertLast(pMenu[i].Id);
        }
        bool bIsRoot = pMenu.Name == EccoConfig::pConfig.BuyMenu.RootNodeName;
        uint iLength = pMenu.length();
        array<Vector2D> tobeinsertIndex = {};
        for(uint i = 0; i < temp.length(); i++){
            if(bIsRoot){
                //每7个留两空
                if(iLength > 9){
                    if(i % 7 == 0)
                        tobeinsertIndex.insertLast(Vector2D(i, MENU_LASTPAGE));
                    else if(i % 8 == 0)
                        tobeinsertIndex.insertLast(Vector2D(i-1, MENU_NEXTPAGE));
                }
            }
            else{
                //每6个留三空
                if(iLength >= 9){
                    if(i % 7 == 0)
                        tobeinsertIndex.insertLast(Vector2D(i, MENU_LASTPAGE));
                    else if(i % 8 == 0)
                        tobeinsertIndex.insertLast(Vector2D(i-1, MENU_NEXTPAGE));
                }
            }
        }
        if(tobeinsertIndex.length() > 0){
            for(uint i = tobeinsertIndex.length()-1; i > 0; i--){
                temp.insertAt(uint(tobeinsertIndex[i].x), int(tobeinsertIndex[i].y));
            }
        }
        if(bIsRoot){
            if(iLength > 9){
                for(uint i = 0; i < (temp.length() % 8);i++){
                    temp.insertLast(MENU_INVALID);
                }
                temp.insertLast(MENU_LASTPAGE);
                temp.insertLast(MENU_NEXTPAGE);
            }
        }
        else{
            temp.insertLast(MENU_BACKUP);
            if(iLength >= 9){
                for(uint i = 0; i < (temp.length() % 8);i++){
                    temp.insertLast(MENU_INVALID);
                }
                temp.insertLast(MENU_LASTPAGE);
                temp.insertLast(MENU_NEXTPAGE);
            }
        }

        NetworkMessage m(MSG_ONE, NetworkMessages::NetworkMessageType(MetaHookChannel), pPlayer.edict());
            m.WriteByte(EccoMenuType);
            m.WriteByte(temp.length());
            for(uint i = 0; i < temp.length(); i++){
                m.WriteLong(temp[i]);
            }
        m.End();
        return HOOK_CONTINUE;
    }
}
}
