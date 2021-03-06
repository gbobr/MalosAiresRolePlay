#if defined _isamp_mano_included
	#endinput
#endif
#define _isamp_mano_included

//==============================MODELADO DE MANO================================

//================================CONSTANTES====================================

// #define HAND_LEFT             	0
// #define HAND_RIGHT              1

#define SAVE_UPDATE             1
#define SAVE_CREATE             2
#define SAVE_DESTROY            3

//============================VARIABLES INTERNAS================================

enum SlotInfo {
	Item,
	Amount,
};

new HandInfo[MAX_PLAYERS][2][SlotInfo];

//=======================IMPLEMENTACI�N DE FUNCIONES============================

stock GetHandItem(playerid, hand)
{
	return HandInfo[playerid][hand][Item];
}

stock GetHandParam(playerid, hand)
{
	return HandInfo[playerid][hand][Amount];
}

stock SetHandItemAndParam(playerid, hand, itemid, param)
{
	if(GetHandItem(playerid, hand) > 0) // Si quiere sobreescribir el slot teniendo algo dentro
	{
	    DeleteHandItem(playerid, hand);
		HandInfo[playerid][hand][Item] = 0;
		HandInfo[playerid][hand][Amount] = 0;
	}
	if(itemid > 0)
	{
	    HandInfo[playerid][hand][Item] = itemid;
		HandInfo[playerid][hand][Amount] = param;
		LoadHandItem(playerid, hand);
	}
}

stock SearchHandsForItem(playerid, itemid)
{
	if(HandInfo[playerid][HAND_RIGHT][Item] == itemid)
	    return HAND_RIGHT;
	else if(HandInfo[playerid][HAND_LEFT][Item] == itemid)
	    return HAND_LEFT;
	else
	    return -1;
}

stock SearchFreeHand(playerid)
{
	if(HandInfo[playerid][HAND_RIGHT][Item] == 0)
	    return HAND_RIGHT;
	else if(HandInfo[playerid][HAND_LEFT][Item] == 0)
	    return HAND_LEFT;
	else
	    return -1;
}

stock ResetHands(playerid)
{
	HandInfo[playerid][HAND_LEFT][Item] = 0;
	HandInfo[playerid][HAND_LEFT][Amount] = 0;
	HandInfo[playerid][HAND_RIGHT][Item] = 0;
	HandInfo[playerid][HAND_RIGHT][Amount] = 0;
	return 1;
}

stock PrintHandsForPlayer(playerid, targetid)
{
	new rightitem = GetHandItem(playerid, HAND_RIGHT),
	    leftitem = GetHandItem(playerid, HAND_LEFT);

	SendClientMessage(targetid, COLOR_WHITE, "======================[En la mano]=====================");
 	if(rightitem > 0)
		SendFMessage(targetid, COLOR_WHITE, "[Mano derecha] - Item: %s - %s: %d", GetItemName(rightitem), GetItemParamName(rightitem), GetHandParam(playerid, HAND_RIGHT));
	else
	    SendClientMessage(targetid, COLOR_WHITE, "[Mano derecha] - Nada");
	if(leftitem > 0)
		SendFMessage(targetid, COLOR_WHITE, "[Mano izquierda] - Item: %s - %s: %d", GetItemName(leftitem), GetItemParamName(leftitem), GetHandParam(playerid, HAND_LEFT));
	else
	    SendClientMessage(targetid, COLOR_WHITE, "[Mano izquierda] - Nada");
	SendClientMessage(targetid, COLOR_WHITE, "=====================================================");
	return 1;
}

//=================BORRADO DE ARMAS AL MORIR SI NO ES COPDUTY===================

ResetHandsWeapons(playerid)
{
	if(HandInfo[playerid][HAND_LEFT][Item] > 0 && GetItemType(HandInfo[playerid][HAND_LEFT][Item]) == ITEM_WEAPON)
        SetHandItemAndParam(playerid, HAND_LEFT, 0, 0);
	if(HandInfo[playerid][HAND_RIGHT][Item] > 0 && GetItemType(HandInfo[playerid][HAND_RIGHT][Item]) == ITEM_WEAPON)
		SetHandItemAndParam(playerid, HAND_RIGHT, 0, 0);
	return 1;
}

//=======DESTRUCTOR DE LOS CONTENEDORES EN MEMORIA ASOCIADOS A LAS MANOS========

DestroyPlayerHands(playerid)
{
    if(GetItemType(HandInfo[playerid][HAND_RIGHT][Item]) == ITEM_CONTAINER)
	{
	    Container_Destroy(HandInfo[playerid][HAND_RIGHT][Amount]);
	}
    if(GetItemType(HandInfo[playerid][HAND_LEFT][Item]) == ITEM_CONTAINER)
	{
	    Container_Destroy(HandInfo[playerid][HAND_LEFT][Amount]);
	}
	return 1;
}

//==========[Sincronizacion del arma en mano con las balas gastadas]============

SynchronizeWeaponAmmo(playerid, ammo)
{
	HandInfo[playerid][HAND_RIGHT][Amount] = ammo;
	if(ammo <= 0) // Se le termin� el arma
	{
	    SendFMessage(playerid, COLOR_WHITE, "[OOC] Se te ha borrado el arma ya que llegaste a las %d municiones, y se te han acabado.", ammo);
	    SetHandItemAndParam(playerid, HAND_RIGHT, 0, 0);
	}
}

//==================================COMANDOS====================================

stock GiveItemFromPlayerToPlayer(playerid, playerhand, targetid)
{
 	new itemid = GetHandItem(playerid, playerhand),
	 	str[128],
		targetfreehand;

	if(itemid == 0)
 		return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes ning�n item en esa mano.");
    if(targetid == playerid || !ProxDetectorS(2.0, playerid, targetid))
	    return SendClientMessage(playerid, COLOR_YELLOW2, "�Jugador inv�lido o se encuentra muy lejos!");
	if(GetPVarInt(playerid, "cantSaveItems") == 1)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "�Debes esperar un tiempo antes de volver a interactuar con otro �tem!");
	if(GetPVarInt(playerid, "GrupoPaintball") != 0)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con un arma de Paintball");
	if(itemid == WEAPON_SILENCED && isPlayerUsingTazer(playerid))
   		return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con el tazer.");
	if(itemid == ITEM_ID_TELEFONO_CELULAR)
   		return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con tu tel�fono");
	targetfreehand = SearchFreeHand(targetid);
	if(targetfreehand == -1)
		return SendClientMessage(playerid, COLOR_YELLOW2, "El sujeto tiene ambas manos ocupadas y no puede agarrar nada m�s.");

	SetHandItemAndParam(targetid, targetfreehand, itemid, GetHandParam(playerid, playerhand));
	SetHandItemAndParam(playerid, playerhand, 0, 0);
	format(str, sizeof(str), "le entrega un/a %s a", GetItemName(itemid));
 	PlayerPlayerActionMessage(playerid, targetid, 15.0, str);
	SetPVarInt(playerid, "cantSaveItems", 1);
	SetTimerEx("cantSaveItems", 2000, false, "i", playerid);
	return 1;
}

CMD:dar(playerid, params[])
{
	new targetid;

	if(sscanf(params, "u", targetid))
		return SendClientMessage(playerid, COLOR_GREY, "{5CCAF1}[Sintaxis]:{C8C8C8} /dar [ID/Jugador]");

	GiveItemFromPlayerToPlayer(playerid, HAND_RIGHT, targetid);
	return 1;
}

CMD:dari(playerid, params[])
{
	new targetid;

	if(sscanf(params, "u", targetid))
		return SendClientMessage(playerid, COLOR_GREY, "{5CCAF1}[Sintaxis]:{C8C8C8} /dari [ID/Jugador]");

	GiveItemFromPlayerToPlayer(playerid, HAND_LEFT, targetid);
	return 1;
}

CMD:mc(playerid, params[])
{
	cmd_mano(playerid, "cambiar");
	return 1;
}

CMD:mano(playerid, params[])
{
	new command[32];
	    
	if(sscanf(params, "s[32]", command))
	{
		SendClientMessage(playerid, COLOR_LIGHTYELLOW2, "{5CCAF1}[Sintaxis]:{C8C8C8} /mano [comando]");
        SendClientMessage(playerid, COLOR_LIGHTYELLOW2, "{5CCAF1}[Comandos]:{C8C8C8} usar - tirar - tirari - agarrar - agarrari - cambiar - combinar - ver - guardar");
		PrintHandsForPlayer(playerid, playerid);
    } else {
		if(strcmp(command, "cambiar", true) == 0) {
		
		    new rightitem = GetHandItem(playerid, HAND_RIGHT),
		        rightparam = GetHandParam(playerid, HAND_RIGHT),
		        leftitem = GetHandItem(playerid, HAND_LEFT),
		        leftparam = GetHandParam(playerid, HAND_LEFT);
		        
            if(rightitem == WEAPON_SILENCED && isPlayerUsingTazer(playerid))
	    		return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con el tazer. Utiliza /guardartazer.");
			if(rightitem == ITEM_ID_TELEFONO_CELULAR)
	    		return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con el telefono celular.");
			if(GetPVarInt(playerid, "GrupoPaintball") != 0)
	            return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con un arma de Paintball");
            if(GetPVarInt(playerid, "cantSaveItems") == 1)
 				return SendClientMessage(playerid, COLOR_YELLOW2, "�Debes esperar un tiempo antes de volver a interactuar con otro �tem!");
	    		
          	SetHandItemAndParam(playerid, HAND_RIGHT, leftitem, leftparam);
          	SetHandItemAndParam(playerid, HAND_LEFT, rightitem, rightparam);
          	SetPVarInt(playerid, "cantSaveItems", 1);
			SetTimerEx("cantSaveItems", 3000, false, "i", playerid);
			SendClientMessage(playerid, COLOR_WHITE, "Has intercambiado de una mano a otra los �tems que estabas sosteniendo.");

		} else if(strcmp(command, "combinar", true) == 0) {

		    new rightitem = GetHandItem(playerid, HAND_RIGHT),
		        rightparam = GetHandParam(playerid, HAND_RIGHT),
		        leftitem = GetHandItem(playerid, HAND_LEFT),
		        leftparam = GetHandParam(playerid, HAND_LEFT);

			if(rightitem == WEAPON_SILENCED && isPlayerUsingTazer(playerid))
	    		return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con el tazer. Utiliza /guardartazer.");
			if(rightitem == 0 || leftitem == 0)
			    return SendClientMessage(playerid, COLOR_YELLOW2, "Para usar este comando debes tener la misma arma en ambas manos.");
			if(GetPVarInt(playerid, "GrupoPaintball") != 0)
	            return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con un arma de Paintball");
			if(GetItemType(rightitem) != ITEM_WEAPON || GetItemType(leftitem) != ITEM_WEAPON)
            	return SendClientMessage(playerid, COLOR_YELLOW2, "Para usar este comando debes tener la misma arma en ambas manos.");
			if(rightitem != leftitem)
				return SendClientMessage(playerid, COLOR_YELLOW2, "Para usar este comando debes tener la misma arma en ambas manos.");
			if(rightparam + leftparam > 400)
            	return SendClientMessage(playerid, COLOR_YELLOW2, "La cantidad conjunta de balas no puede superar las 400.");

			SetHandItemAndParam(playerid, HAND_LEFT, 0, 0);
			SetHandItemAndParam(playerid, HAND_RIGHT, rightitem, rightparam + leftparam);

		} else if(strcmp(command, "tirar", true) == 0) {

			DropObject(playerid, HAND_RIGHT);

		} else if(strcmp(command, "tirari", true) == 0) {
		
			DropObject(playerid, HAND_LEFT);

		} else if(strcmp(command, "agarrar", true) == 0) {

			TakeObject(playerid, HAND_RIGHT);

		} else if(strcmp(command, "agarrari", true) == 0) {

			TakeObject(playerid, HAND_LEFT);
			
		} else if(strcmp(command, "ver", true) == 0) {

			new item = GetHandItem(playerid, HAND_RIGHT),
			    param = GetHandParam(playerid, HAND_RIGHT);
			    
			if(GetItemType(item) != ITEM_CONTAINER)
			    return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes ning�n �tem en la mano derecha del tipo contenedor.");
			if(!Container_Show(playerid, CONTAINER_TYPE_ITEM, param, playerid))
				return SendClientMessage(playerid, COLOR_YELLOW2, "[SCRIPT ERROR]: Error al mostrar el contenedor del item. Reportar a un administador.");

		} else if(strcmp(command, "guardar", true) == 0) {

			new left_item = GetHandItem(playerid, HAND_LEFT),
			    left_param = GetHandParam(playerid, HAND_LEFT);

			if(GetItemType(GetHandItem(playerid, HAND_RIGHT)) != ITEM_CONTAINER)
			    return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes ning�n �tem en la mano derecha del tipo contenedor.");
			if(left_item == 0)
			    return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes ning�n �tem en la mano izquierda para guardar.");
			if(left_item == WEAPON_SILENCED && isPlayerUsingTazer(playerid))
			    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con el tazer.");
			if(GetPVarInt(playerid, "GrupoPaintball") != 0)
			    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con un arma de Paintball");
			if(GetPVarInt(playerid, "disabled") != DISABLE_NONE)
			    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo en este momento.");
			if(PlayerInfo[playerid][pHealth] < 25.0)
				return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes guardar un item agonizando.");
			if(!Container_AddItemAndParam(GetHandParam(playerid, HAND_RIGHT), left_item, left_param))
		    	return SendClientMessage(playerid, COLOR_YELLOW2, "No hay suficiente espacio libre en el contenedor.");

			SetHandItemAndParam(playerid, HAND_LEFT, 0, 0); // Borrado l�gico y grafico de la mano.

			SendFMessage(playerid, COLOR_WHITE, "Has guardado un/a %s en el contenedor que tienes en la mano derecha.", GetItemName(left_item));

			SetPVarInt(playerid, "cantSaveItems", 1);
			SetTimerEx("cantSaveItems", 2000, false, "i", playerid);
				
		} else if(strcmp(command, "usar", true) == 0) {

			if(GetHandItem(playerid, HAND_RIGHT) == ITEM_ID_SANDWICH) {
	            PlayerActionMessage(playerid, 15.0, "come un pedazo de su s�ndwich de jam�n y queso.");
	            PlayerEat(playerid, 10);
	            if(GetHandParam(playerid, HAND_RIGHT) - 1 > 0)
	            	SetHandItemAndParam(playerid, HAND_RIGHT, ITEM_ID_SANDWICH, GetHandParam(playerid, HAND_RIGHT) - 1);
					else
					SetHandItemAndParam(playerid, HAND_RIGHT, 0, 0); // Borrado l�gico y grafico
	        } else if(GetHandItem(playerid, HAND_RIGHT) == ITEM_ID_ALFAJOR) {
         		PlayerActionMessage(playerid, 15.0, "comienza a comer un alfajor.");
		        PlayerEat(playerid, 15);
		        if(GetHandParam(playerid, HAND_RIGHT) - 1 > 0)
		       	   	SetHandItemAndParam(playerid, HAND_RIGHT, ITEM_ID_ALFAJOR, GetHandParam(playerid, HAND_RIGHT) - 1);
					else
					SetHandItemAndParam(playerid, HAND_RIGHT, 0, 0); // Borrado l�gico y grafico
			} else if(GetHandItem(playerid, HAND_RIGHT) == ITEM_ID_AGUAMINERAL) {
                if(GetHandParam(playerid, HAND_RIGHT) > 0) {
				  PlayerActionMessage(playerid, 15.0, "toma un trago de su botella de agua.");
			      PlayerDrink(playerid, 10);
        	      SetHandItemAndParam(playerid, HAND_RIGHT, ITEM_ID_AGUAMINERAL, GetHandParam(playerid, HAND_RIGHT) - 1);
			      } else
	              SendClientMessage(playerid, COLOR_YELLOW2, "�La botella esta vacia! Necesitas llenar tu botella para usarla.");
           	} else if(GetHandItem(playerid, HAND_RIGHT) == ITEM_ID_CERVEZA) {
				PlayerActionMessage(playerid, 15.0, "toma un trago de su botella de cerveza.");
			    PlayerDrink(playerid, 10);
			    PlayerDrinkAlcohol(playerid, 4);
			    if(GetHandParam(playerid, HAND_RIGHT) - 1 > 0)
			      	SetHandItemAndParam(playerid, HAND_RIGHT, ITEM_ID_CERVEZA, GetHandParam(playerid, HAND_RIGHT) - 1);
				  else
					SetHandItemAndParam(playerid, HAND_RIGHT, 0, 0); // Borrado l�gico y grafico
				} else
  				SendClientMessage(playerid, COLOR_YELLOW2, "�Item inv�lido o inexistente!");
		}
	}
	return 1;
}

Dialog:Dlg_Show_Item_Container(playerid, response, listitem, inputtext[])
{
	new container_id = Container_Selection[playerid][csId];

	ResetContainerSelection(playerid);

	if(response)
	{
        new itemid,
            itemparam,
            str[128];

		if(GetHandItem(playerid, HAND_LEFT) != 0)
		    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes tomar el item ya que tienes la mano izquierda ocupada.");

		if(Container_TakeItem(container_id, listitem, itemid, itemparam))
		{
			SetHandItemAndParam(playerid, HAND_LEFT, itemid, itemparam); // Creaci�n l�gica y grafica en la mano.

            SetPVarInt(playerid, "cantSaveItems", 1);
			SetTimerEx("cantSaveItems", 2000, false, "i", playerid);

			format(str, sizeof(str), "toma un/a %s del contenedor de su mano derecha.", GetItemName(itemid));
			PlayerActionMessage(playerid, 15.0, str);
		}
		else
		{
	        SendClientMessage(playerid, COLOR_YELLOW2, "Contenedor vacio o el slot es inv�lido.");
		}
	}
	return 1;
}
