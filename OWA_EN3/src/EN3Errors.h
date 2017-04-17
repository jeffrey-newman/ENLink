//
// Created by a1091793 on 13/4/17.
// Actually just ripping out the en2 error code. No idea if they correspond.

#ifndef EN2LINK_EN3ERRORS_H
#define EN2LINK_EN3ERRORS_H




/*------------------- Error Messages --------------------*/
#define ERR101 "System Error 101: insufficient memory available."
#define ERR102 "System Error 102: no network data available."
#define ERR103 "System Error 103: hydraulics not initialized."
#define ERR104 "System Error 104: no hydraulics for water quality analysis."
#define ERR105 "System Error 105: water quality not initialized."
#define ERR106 "System Error 106: no results saved to report on."
#define ERR107 "System Error 107: hydraulics supplied from external file."
#define ERR108 "System Error 108: cannot use external file while hydraulics solver is active."
#define ERR109 "System Error 109: cannot change time parameter when solver is active."
#define ERR110 "System Error 110: cannot solve network hydraulic equations."
#define ERR120 "System Error 120: cannot solve water quality transport equations."

#define ERR200 "Input Error 200: one or more errors in input file."
#define ERR201 \
 "Input Error 201: syntax error in following line of [%s] section:"
#define ERR202 "Input Error 202: %s %s contains illegal numeric value."
#define ERR203 "Input Error 203: %s %s refers to undefined node."
#define ERR204 "Input Error 204: %s %s refers to undefined link."
#define ERR205 "Input Error 205: %s %s refers to undefined time pattern."
#define ERR206 "Input Error 206: %s %s refers to undefined curve."
#define ERR207 "Input Error 207: %s %s attempts to control a CV."

#define ERR208 "Input Error 208: %s specified for undefined Node %s."
#define ERR209 "Input Error 209: illegal %s value for Node %s."
#define ERR210 "Input Error 210: %s specified for undefined Link %s."
#define ERR211 "Input Error 211: illegal %s value for Link %s."
#define ERR212 "Input Error 212: trace node %.0s %s is undefined."
#define ERR213 "Input Error 213: illegal option value in [%s] section:"
#define ERR214 \
 "Input Error 214: following line of [%s] section contains too many characters:"
#define ERR215 "Input Error 215: %s %s is a duplicate ID."
#define ERR216 "Input Error 216: %s data specified for undefined Pump %s."
#define ERR217 "Input Error 217: invalid %s data for Pump %s."
#define ERR219 "Input Error 219: %s %s illegally connected to a tank."
#define ERR220 "Input Error 220: %s %s illegally connected to another valve."

/*** Updated on 10/25/00 ***/
#define ERR222 "Input Error 222: %s %s has same start and end nodes."

#define ERR223 "Input Error 223: not enough nodes in network"
#define ERR224 "Input Error 224: no tanks or reservoirs in network."
#define ERR225 "Input Error 225: invalid lower/upper levels for Tank %s."
#define ERR226 "Input Error 226: no head curve supplied for Pump %s."
#define ERR227 "Input Error 227: invalid head curve for Pump %s."
#define ERR230 "Input Error 230: Curve %s has nonincreasing x-values."
#define ERR233 "Input Error 233: Node %s is unconnected."
#define ERR240 "Input Error 240: %s %s refers to undefined source."
#define ERR241 "Input Error 241: %s %s refers to undefined control."
#define ERR250 "Input Error 250: function call contains invalid format."
#define ERR251 "Input Error 251: function call contains invalid parameter code."

#define ERR253 "Input Error 253: Function call error - No such demand category index."
#define ERR254 "Input Error 254: Function call error - Node have no coordinates."
#define ERR255 "Input Error 255: Function call error - Coordinates were not loaded."

#define ERR301 "File Error 301: identical file names."
#define ERR302 "File Error 302: cannot open input file."
#define ERR303 "File Error 303: cannot open report file."
#define ERR304 "File Error 304: cannot open binary output file."
#define ERR305 "File Error 305: cannot open hydraulics file."
#define ERR306 "File Error 306: hydraulics file does not match network data."
#define ERR307 "File Error 307: cannot read hydraulics file."
#define ERR308 "File Error 308: cannot save results to file."
#define ERR309 "File Error 309: cannot save results to report file."

#define ERR401 "Sync Error 401: Qstep is not dividable by Hstep. Can't sync."

#define R_ERR201 "Input Error 201: syntax error in following line of "
#define R_ERR202 "Input Error 202: illegal numeric value in following line of "
#define R_ERR203 "Input Error 203: undefined node in following line of "
#define R_ERR204 "Input Error 204: undefined link in following line of "
#define R_ERR207 "Input Error 207: attempt to control a CV in following line of "

#define R_ERR221 "Input Error 221: mis-placed clause in following line of "

/*-------------------- Specific Warning Messages -------------------------*/
#define WARN01 "WARNING: System unbalanced at %s hrs."
#define WARN02 \
"WARNING: Maximum trials exceeded at %s hrs. System may be unstable."
#define WARN03a "WARNING: Node %s disconnected at %s hrs"
#define WARN03b "WARNING: %d additional nodes disconnected at %s hrs"
#define WARN03c "WARNING: System disconnected because of Link %s"
#define WARN04  "WARNING: Pump %s %s at %s hrs."
#define WARN05  "WARNING: %s %s %s at %s hrs."
#define WARN06  "WARNING: Negative pressures at %s hrs."

/*-------------------- General Warning Messages -------------------------*/
#define WARN1 "WARNING: System hydraulically unbalanced."
#define WARN2 "WARNING: System may be hydraulically unstable."
#define WARN3 "WARNING: System disconnected."
#define WARN4 "WARNING: Pumps cannot deliver enough flow or head."
#define WARN5 "WARNING: Valves cannot deliver enough flow."
#define WARN6 "WARNING: System has negative pressures."

#define   t_FUNCCALL    "function call"

char *geterrmsg(int errcode)
/*----------------------------------------------------------------
**  Input:   errcode = error code
**  Output:  none
**  Returns: pointer to string with error message
**  Purpose: retrieves text of error message
**----------------------------------------------------------------
*/
{
    switch (errcode)
    {                                   /* Warnings */
/*
      case 1:     strcpy(Msg,WARN1);   break;
      case 2:     strcpy(Msg,WARN2);   break;
      case 3:     strcpy(Msg,WARN3);   break;
      case 4:     strcpy(Msg,WARN4);   break;
      case 5:     strcpy(Msg,WARN5);   break;
      case 6:     strcpy(Msg,WARN6);   break;
*/
        /* System Errors */
        case 101:   strcpy(Msg,ERR101);  break;
        case 102:   strcpy(Msg,ERR102);  break;
        case 103:   strcpy(Msg,ERR103);  break;
        case 104:   strcpy(Msg,ERR104);  break;
        case 105:   strcpy(Msg,ERR105);  break;
        case 106:   strcpy(Msg,ERR106);  break;
        case 107:   strcpy(Msg,ERR107);  break;
        case 108:   strcpy(Msg,ERR108);  break;
        case 109:   strcpy(Msg,ERR109);  break;
        case 110:   strcpy(Msg,ERR110);  break;
        case 120:   strcpy(Msg,ERR120);  break;

            /* Input Errors */
        case 200:  strcpy(Msg,ERR200);   break;
        case 223:  strcpy(Msg,ERR223);   break;
        case 224:  strcpy(Msg,ERR224);   break;

            /* Toolkit function errors */
        case 202:  sprintf(Msg,ERR202,t_FUNCCALL,""); break;
        case 203:  sprintf(Msg,ERR203,t_FUNCCALL,""); break;
        case 204:  sprintf(Msg,ERR204,t_FUNCCALL,""); break;
        case 205:  sprintf(Msg,ERR205,t_FUNCCALL,""); break;
        case 207:  sprintf(Msg,ERR207,t_FUNCCALL,""); break;
        case 240:  sprintf(Msg,ERR240,t_FUNCCALL,""); break;
        case 241:  sprintf(Msg,ERR241,t_FUNCCALL,""); break;
        case 250:  sprintf(Msg,ERR250);  break;
        case 251:  sprintf(Msg,ERR251);  break;
        case 253:  sprintf(Msg,ERR253);  break;
        case 254:  sprintf(Msg,ERR254);  break;
        case 255:  sprintf(Msg,ERR255);  break;

            /* File Errors */
        case 301:  strcpy(Msg,ERR301);   break;
        case 302:  strcpy(Msg,ERR302);   break;
        case 303:  strcpy(Msg,ERR303);   break;
        case 304:  strcpy(Msg,ERR304);   break;
        case 305:  strcpy(Msg,ERR305);   break;
        case 306:  strcpy(Msg,ERR306);   break;
        case 307:  strcpy(Msg,ERR307);   break;
        case 308:  strcpy(Msg,ERR308);   break;
        case 309:  strcpy(Msg,ERR309);   break;

        case 401:  strcpy(Msg,ERR401); break;
        default:   strcpy(Msg,"");
    }
    return(Msg);
}


int  EN3geterror(int errcode, char *errmsg, int n)
{
    switch (errcode)
    {
        case 1:  strncpy(errmsg,WARN1,n);   break;
        case 2:  strncpy(errmsg,WARN2,n);   break;
        case 3:  strncpy(errmsg,WARN3,n);   break;
        case 4:  strncpy(errmsg,WARN4,n);   break;
        case 5:  strncpy(errmsg,WARN5,n);   break;
        case 6:  strncpy(errmsg,WARN6,n);   break;
        default: strncpy(errmsg,geterrmsg(errcode),n);
    }
    if (strlen(errmsg) == 0) return(251);
    else return(0);
}
#endif //EN2LINK_EN3ERRORS_H
