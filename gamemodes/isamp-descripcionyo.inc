#if defined _isamp_descripcion_yo
#endinput
#endif 
#define _isamp_descripcion_yo
new Text3D:DescLabel[MAX_PLAYERS]; 
new GCounter[MAX_PLAYERS] = 0; 
new textodescripcion[70][MAX_PLAYERS];
CMD:yo(playerid, params[]){ 
new 
    str[120], 
    str2[120], 
	str3[120],
    Float:X, 
    Float:Y, 
    Float:Z; 
if(sscanf(params, "s[70]", textodescripcion[playerid])){ 
    SendClientMessage(playerid, COLOR_GREY, "{5CCAF1}[Sintaxis]:{C8C8C8} /yo [descripci�n]"); 
    SendClientMessage(playerid, COLOR_GREY, "No olvides que la descripci�n no puede ser mayor a 65 caracteres."); 
	if(GCounter[playerid] == 1){ 
	format(str3, sizeof(str3), "{C8C8C8}Tu descripci�n actual es:{FFFFFF} %s", textodescripcion[playerid]); 
	SendClientMessage(playerid, -1 , str3);
	} 
    return 1; 
} 
if(GCounter[playerid] == 0){
if(strlen(textodescripcion[playerid]) > 65) return SendClientMessage(playerid, COLOR_WHITE, "Texto demasiado largo.");
GetPlayerPos(playerid, X, Y, Z); 
format(str, sizeof(str), "%s", textodescripcion[playerid]); 
DescLabel[playerid] = Create3DTextLabel(textodescripcion[playerid], COLOR_RED, X, Y, Z, 10, -1); 
Attach3DTextLabelToPlayer(DescLabel[playerid], playerid, 0.0, 0.0, 0.3); 
SendFMessage(playerid, -1, "{C8C8C8}Tu descripci�n es:{FFFFFF} %s {C8C8C8}us� /yob para eliminarla.", textodescripcion[playerid]); 
GCounter[playerid] = 1; 
} else { 
    Update3DTextLabelText(DescLabel[playerid], COLOR_RED, textodescripcion[playerid]); 
    Attach3DTextLabelToPlayer(DescLabel[playerid], playerid, 0.0, 0.0, 0.3); 
    format(str2, sizeof(str2), "{C8C8C8}Tu descripci�n es:{FFFFFF} %s", textodescripcion[playerid]); 
    SendClientMessage(playerid, -1, str2); 
} 
return 1; 
} 

CMD:yob(playerid, params[]){ 
GCounter[playerid] = 0; 
Delete3DTextLabel(DescLabel[playerid]); 
SendClientMessage(playerid, -1, "Borraste tu descripci�n actual."); 
return 1; 
}  