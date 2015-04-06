#if defined _marp_garbjob_included
	#endinput
#endif
#define _marp_garbjob_included

#define JOB_GARB_TRAVEL_TIME  	10 // Tiempo limite para hacer el recorrido, en minutos.

forward GarbJob_LoadingTimer(playerid, garbcp);

new Float:JOB_GARB_POS_R0[][3] = {
	// basureros ruta oeste
	{2416.5730,-1982.5348,13.0964},
	{2316.1499,-1959.5759,13.1051},
	{2095.5801,-1891.8380,13.0947},
	{2040.2618,-1810.0276,13.1099},
	{1964.1158,-1767.0380,13.1099},
	{1837.2772,-1749.1176,13.1099},
	{1578.1523,-1729.1829,13.1099},
	{1349.9713,-1729.9696,13.1099},
	{1322.6595,-1529.6226,13.1099},
	{1144.6719,-1392.5767,13.3237},
	{932.5922,-1392.6332,12.9839},
	{624.9330,-1430.7775,13.6419},
	{260.6046,-1434.2141,13.2730},
	{471.4320,-1313.2888,15.0369},
	{756.5276,-1065.1980,23.6607},
	{1069.5193,-962.5571,42.2988},
	{1350.2448,-1058.4478,26.3018},
	{1432.6157,-1162.3704,23.3888},
	{1451.5614,-1298.7955,13.1099},
	{1427.3477,-1576.6962,13.0883},
	{1719.5588,-1597.1793,13.0875},
	{1746.7151,-1713.8253,13.1099},
	{1819.0933,-1804.3193,13.1204},
	{1959.1876,-2015.9716,13.1177},
	{2385.3523,-1975.1807,13.1099},
    {2389.2288,-2064.5564,13.2214} 	// entraga 25
};

new Float:JOB_GARB_POS_R1[][3] = {
	// basureros ruta este
	{2416.1792,-2020.9438,13.1418},
	{2476.4333,-1934.5983,13.0717},
	{2711.6902,-1972.1912,13.0594},
	{2771.5774,-1963.7712,13.0783},
	{2645.8135,-1598.2715,12.0833},
	{2645.6782,-1362.1589,32.2359},
	{2595.5840,-1253.8698,46.7980},
	{2703.3892,-1186.4670,68.9670},
	{2575.7156,-1045.0909,69.1411},
	{2255.7195,-1047.6941,51.5216},
	{1974.7074,-1074.7692,24.5354},
	{1901.5074,-1133.1996,24.0768},
	{2025.8148,-1072.6514,24.2742},
	{2159.2893,-1223.5388,23.5435},
	{2268.2837,-1261.4368,23.5436},
	{2209.7693,-1432.7487,23.5503},
	{2364.6401,-1526.9608,23.5552},
	{2397.0527,-1730.0308,13.1099},
	{2129.3398,-1749.9578,13.1302},
	{2079.8037,-1795.3230,13.1156},
	{2152.0408,-1896.6810,13.1071},
	{2297.5706,-1896.5006,13.1260},
	{2311.2969,-1956.7001,13.0992},
	{2386.9607,-1975.1156,13.1099},
	{2411.1948,-2013.1652,13.0868},
    {2389.2288,-2064.5564,13.2214} // entraga 25
};

new Float:jobVehicleHealth[MAX_PLAYERS],
    jobVehicle[MAX_PLAYERS],
    jobCheckpoint[MAX_PLAYERS],
    jobRoute[MAX_PLAYERS];

GarbJob_StartWork(playerid)
{
    new vehicleid = GetPlayerVehicleID(playerid);

	if(getVehicleType(vehicleid) != VEH_JOB || getVehicleJob(vehicleid) != JOB_GARB)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "�Debes ingresar a un cami�n recolector!");
	if(PlayerInfo[playerid][pCarLic] == 0)
		return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes una licencia de conducir. No podr�s trabajar sin ella.");
		
	new route = random(2);

	if(route == 0)
	{
 		SetPlayerCheckpoint(playerid, JOB_GARB_POS_R0[0][0], JOB_GARB_POS_R0[0][1], JOB_GARB_POS_R0[0][2], 5.0);
	}
	else if(route == 1) 
	{
	    SetPlayerCheckpoint(playerid, JOB_GARB_POS_R1[0][0], JOB_GARB_POS_R1[0][1], JOB_GARB_POS_R1[0][2], 5.0);
	}
	PlayerActionMessage(playerid, 15.0, "ha encendido el motor del veh�culo.");
	SendClientMessage(playerid, COLOR_WHITE, "Completa el recorrido para dejar la ciudad libre de basura. Al finalizar ser�s recompensado por tu trabajo.");
    SendFMessage(playerid, COLOR_WHITE, "Tienes %d minutos para terminar el recorrido. Si llegas m�s tarde no se te pagar�, y tu imagen laboral empeorar�.", JOB_GARB_TRAVEL_TIME);
    SendClientMessage(playerid, COLOR_WHITE, "Recuerda tambi�n que si entregas el cami�n recolector en peor estado del que lo recibiste, tu imagen laboral disminuir�.");
	jobDuty[playerid] = true;
	GetVehicleHealth(vehicleid, jobVehicleHealth[playerid]);
	SetEngine(vehicleid, 1);
	jobVehicle[playerid] = vehicleid;
	jobCheckpoint[playerid] = 0;
	jobRoute[playerid] = route;
	CrearCronometro(playerid, JOB_GARB_CRONOMETRO, JOB_GARB_TRAVEL_TIME * 60);
	return 1;
}

GarbJob_IsPlayerWorking(playerid, vehicleid)
{
	if(PlayerInfo[playerid][pJob] == JOB_GARB && jobDuty[playerid] && VehicleInfo[vehicleid][VehJob] == JOB_GARB)
	    return 1;
	return 0;
}

GarbJob_PlayerEnterCheckpoint(playerid)
{
    new gbCP = jobCheckpoint[playerid],
		vehicleid = GetPlayerVehicleID(playerid);

	if(jobVehicle[playerid] != vehicleid)
	{
	    return SendClientMessage(playerid, COLOR_YELLOW2, "�Debes estar en el cami�n de transporte con el que empezaste a trabajar!");
	}
    if(gbCP == 26)
	{
		SendClientMessage(playerid, COLOR_WHITE, "Vaciando cami�n...");
		TogglePlayerControllable(playerid, false);
		SetPVarInt(playerid, "garbT", SetTimerEx("GarbJob_LoadingTimer", 6000, false, "ii", playerid, gbCP));
		BorrarCronometro(playerid);
    }
	else
	{
        TogglePlayerControllable(playerid, false);
        SetPVarInt(playerid, "garbT", SetTimerEx("GarbJob_LoadingTimer", 2000, false, "ii", playerid, gbCP));
        GameTextForPlayer(playerid, "Cargando basura...", 2000, 4);
		jobCheckpoint[playerid]++;
		if(jobRoute[playerid] == 0)
		{
 			SetPlayerCheckpoint(playerid, JOB_GARB_POS_R0[gbCP+1][0], JOB_GARB_POS_R0[gbCP+1][1], JOB_GARB_POS_R0[gbCP+1][2], 5.0);
		}
		else if(jobRoute[playerid] == 1)
		{
		    SetPlayerCheckpoint(playerid, JOB_GARB_POS_R1[gbCP+1][0], JOB_GARB_POS_R1[gbCP+1][1], JOB_GARB_POS_R1[gbCP+1][2], 5.0);
		}
    }
	return 1;
}

GarbJob_TravelTimeLimit(playerid)
{
    new vehicleid = GetPlayerVehicleID(playerid);
    
    PlayerInfo[playerid][pCantWork] = 1;
    jobDuty[playerid] = false;
    if(jobVehicle[playerid] == vehicleid)
    {
	    SetEngine(vehicleid, 0);
		SetVehicleToRespawn(vehicleid);
	}
	SetPlayerSkin(playerid, PlayerInfo[playerid][pSkin]);
	SendClientMessage(playerid, COLOR_WHITE, "Te has pasado del tiempo l�mite: tu jefe se ha enojado y adem�s no recibir�s ninguna paga el d�a de hoy.");
	UpdatePlayerJobLastWorked(playerid);
	PlayerJobInfo[playerid][pReputation] -= 30;
}

public GarbJob_LoadingTimer(playerid, garbcp)
{
    TogglePlayerControllable(playerid, true);
    if(garbcp == 25)
	{
		SendClientMessage(playerid, COLOR_WHITE, "Vuelve y vac�a el cami�n en el dep�sito.");
  	}
  	else if(garbcp == 26)
	{
	    new Float:vHealth;
	    
	    GetVehicleHealth(GetPlayerVehicleID(playerid), vHealth);
	    if(vHealth + 30.0 < jobVehicleHealth[playerid])
	    {
	        PlayerJobInfo[playerid][pReputation] -= floatround(jobVehicleHealth[playerid] - vHealth, floatround_ceil) / 10; // La reputaci�n que baja es proporcional al da�o del veh�culo (un d�cimo)
            PlayerInfo[playerid][pPayCheck] -= floatround(jobVehicleHealth[playerid] - vHealth, floatround_ceil); // Se le paga x$$$ menos correspondientes a la diferencia de vida en el veh�culo.
			SendClientMessage(playerid, COLOR_WHITE, "Tu jefe se enoj� con vos ya que entregaste el cami�n recolector en mal estado.");
		}
		else
		    PlayerJobInfo[playerid][pReputation]++;

		RemovePlayerFromVehicle(playerid);
	    PlayerInfo[playerid][pCantWork] = 1;
	    jobDuty[playerid] = false;
	    SetPlayerSkin(playerid, PlayerInfo[playerid][pSkin]);
	    SetEngine(GetPlayerVehicleID(playerid), 0);
		SetVehicleToRespawn(GetPlayerVehicleID(playerid));
		PlayerActionMessage(playerid, 15.0, "abre las puertas del dep�sito y vac�a el cami�n recolector.");
	    PlayerInfo[playerid][pPayCheck] += GarbJob_GetPaycheck(playerid);
	    PlayerJobInfo[playerid][pTotalEarnings] += GarbJob_GetPaycheck(playerid);
		SendFMessage(playerid, COLOR_WHITE, "�Enhorabuena! has finalizado  tu trabajo, recibir�s $%d en el pr�ximo payday.", GarbJob_GetPaycheck(playerid));
  	    PlayerJobInfo[playerid][pWorkingHours]++;
		UpdatePlayerJobLastWorked(playerid);
        GarbJob_CheckPromotePlayer(playerid);
  	}
	return 1;
}

GarbJob_GetPaycheck(playerid)
{
	new paycheck = 0;

	paycheck += GetJobBaseSalary(JOB_GARB); // Salario base
	paycheck += PlayerJobInfo[playerid][pWorkingHours] / 2; // Extra por antiguedad
	paycheck += (PlayerJobInfo[playerid][pCharge] - 1) * 100; // Extra por cargo

	return paycheck;
}

GarbJob_CheckPromotePlayer(playerid)
{
	if(140 * PlayerJobInfo[playerid][pCharge] >= PlayerJobInfo[playerid][pWorkingHours] && PlayerJobInfo[playerid][pReputation] >= 0 && PlayerJobInfo[playerid][pCharge] < 5) // Minimo de X horas trabajadas (proporcional al siguiente puesto) y de una im�gen laboral neutral.
	{
	    if(PlayerJobInfo[playerid][pWorkingHours] + PlayerJobInfo[playerid][pReputation] / 10 >= 180 * PlayerJobInfo[playerid][pCharge]) // Para ascender hay que llegar a una cantidad de puntos. Nuestro puntaje se suma entre la reputacion y la antiguedad.
	    {
	        PlayerJobInfo[playerid][pCharge]++;
	        SendFMessage(playerid, COLOR_WHITE, "�Enhorabuena! has sido promovido a %s, lo que te garantiza una mejora en tu sueldo. �Buen trabajo! Sigue as�.", GetJobChargeName(JOB_GARB, PlayerJobInfo[playerid][pCharge]));
		}
	}
}