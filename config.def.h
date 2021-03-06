/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx = 3;		  /* border pixel of windows */
static const unsigned int snap = 32;		  /* snap pixel */
static const unsigned int startmenusize = 30;		  /* snap pixel */
static const unsigned int systraypinning = 0; /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 0; /* systray spacing */
static const int systraypinningfailfirst = 1; /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray = 1;			  /* 0 means no systray */
static const int showbar = 1;				  /* 0 means no bar */
static const int topbar = 1;				  /* 0 means bottom bar */
static const char *fonts[] = {"Cantarell-Regular:size=12", "Fira Code Nerd Font:size=12"};

static char col_background[] = "#292f3a"; /* top bar dark background*/
static char col_white[] = "#ffffff";/*white for fonts*/
static char col_pastel_blue[] = "#747c90";/*unsaturated for focused border*/
static char col_light_blue[] = "#4dadd4";/*focused windows d */
static char col_blue[] = "#5294E2";/*focused instantmenu or topbar d */
static char col_dark_blue[] = "#3579CB";/*focused instantmenu or topbar d */
static char col_dark_green[] = "#35CB4B";/*focused instantmenu or topbar d */
static char col_green[] = "#52E167";/*focused instantmenu or topbar d */

static char col_orange[] = "#E1A052";/*focused instantmenu or topbar d */
static char col_dark_orange[] = "#CB8735";/*focused instantmenu or topbar d */

static char col_red[] = "#E1527E";/*focused instantmenu or topbar d */
static char col_dark_red[] = "#CB3563";/*focused instantmenu or topbar d */

static char col_hover_red[] = "#D37492";/*focused instantmenu or topbar d */
static char col_hover_dark_red[] = "#CE577C";/*focused instantmenu or topbar d */

static char col_hover[] = "#596377";/*focused instantmenu or topbar d */
static char col_hover_shadow[] = "#475166";/*focused instantmenu or topbar d */

static char col_hover_blue[] = "#7CA8DC";/*focused instantmenu or topbar d */
static char col_hover_dark_blue[] = "#578BC9";/*focused instantmenu or topbar d */


static const char *colors[][4] = {
	/*               fg         bg         border   	float*/
	[SchemeNorm] = { col_white, col_background, col_pastel_blue, col_green },
	[SchemeSel]  = { col_white, col_blue, col_light_blue, col_green },
	[SchemeHid]  = { col_pastel_blue, col_background, col_pastel_blue, col_green },
	[SchemeTags]  = { col_white, col_blue, col_light_blue, col_dark_blue },
	[SchemeActive]  = { col_white, col_green, col_light_blue, col_dark_green },
	[SchemeAddActive]  = { col_white, col_orange, col_light_blue, col_dark_orange },
	[SchemeEmpty]  = { col_white, col_red, col_light_blue, col_dark_red },
	[SchemeHover]  = { col_white, col_hover, col_light_blue, col_hover_shadow },
	[SchemeClose]  = { col_hover_red, col_red, col_hover_dark_red, col_dark_red },
	[SchemeHoverTags]  = { col_white, col_hover_blue, col_light_blue, col_hover_dark_blue },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20" };
/* ffox, programming1, term, music, steam, folder, play icon, document, message  */
static const char *tagsalt[] = { "", "{}", "$", "", "", "", "", "", "" };

static const char *upvol[] = {"/opt/instantos/menus/ex/p.sh", "+5", NULL};
static const char *downvol[] = {"/opt/instantos/menus/ex/p.sh", "-5", NULL};
static const char *mutevol[] = {"/opt/instantos/menus/ex/p.sh", "m", NULL};

static const char *upbright[] = {"/opt/instantos/menus/ex/b.sh", "+", NULL};
static const char *downbright[] = {"/opt/instantos/menus/ex/b.sh", "-", NULL};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{"Pavucontrol", NULL,     NULL,       0,            1,           -1},
	{"Onboard", NULL,     NULL,       0,                1,           -1},
	{"Welcome.py", NULL,     NULL,        0,            1,           -1},
	{"ROX-Filer", NULL,     NULL,        0,            0,           -1},
};

/* layout(s) */
static const float mfact = 0.55;  /* factor of master area size [0.05..0.95] */
static const int nmaster = 1;	 /* number of clients in master area */
static const int resizehints = 1; /* 1 means respect size hints in tiled resizals */
static const int decorhints  = 1;    /* 1 means respect decoration hints */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "+",        tile },    /* first entry is default */
	{ "#",        grid },
	{ "-",        NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "|||",      tcl },
	{ "H[]",      deck },
	{ "O",        overviewlayout },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG)                                          \
		{MODKEY, KEY, view, {.ui = 1 << TAG}},                     \
		{MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}}, \
		{MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG}},          \
		{MODKEY | Mod1Mask, KEY, followtag, {.ui = 1 << TAG}},          \
		{MODKEY | ControlMask | ShiftMask, KEY, toggletag, {.ui = 1 << TAG}},


#define SHCMD(cmd)                                           \
	{                                                        \
		.v = (const char *[]) { "/bin/sh", "-c", cmd, NULL } \
	}

/* commands */
static char instantmenumon[2] = "0"; /* component of instantmenucmd, manipulated in spawn() */
static const char *instantmenucmd[] = {"instantmenu_run", NULL};
static const char *roficmd[] = {"rofi", "-sort", "-show", "run", NULL};
static const char *instantmenustcmd[] = {"instantmenu_run_st", NULL};
static const char *termcmd[] = {"st", NULL};
static const char *quickmenucmd[] = {"quickmenu", NULL};
static const char *instantassistcmd[] = {"instantassist", NULL};
static const char *nautiluscmd[] = {"nautilus", NULL};
static const char *slockcmd[] = {"ilock", NULL};
static const char *oslockcmd[] = {"instantlock", "-o", NULL};
static const char *slockmcmd[] = {"ilock", "message", NULL};
static const char *helpcmd[] = {"instanthotkeys", "gui", NULL};
static const char *iswitchcmd[] = {"iswitch", NULL};
static const char *instantswitchcmd[] = {"rofi", "-show", "window", "-kb-row-down", "Alt+Tab,Down", "-kb-row-up", "Alt+Ctrl+Tab,Up", "-kb-accept-entry", "!Alt_L,!Alt+Tab,Return", "-me-select-entry", "", "-me-accept-entry", "MousePrimary", NULL};
static const char *caretinstantswitchcmd[] = {"rofi", "-show", "window", "-kb-row-down", "Alt+Tab,Down", "-kb-row-up", "Alt+Ctrl+Tab,Up", "-kb-accept-entry", "!Alt_L,!Alt+Tab,Return", "-me-select-entry", "", "-me-accept-entry", "MousePrimary", "-theme", "/usr/share/instantdotfiles/rofi/appmenu.rasi", NULL};
static const char *onboardcmd[] = {"onboard", NULL};
static const char *instantshutdowncmd[] = {"instantshutdown", NULL};
static const char *notifycmd[] = {"instantnotify", NULL};
static const char *rangercmd[] = { "urxvt", "-e", "ranger", NULL };
static const char *panther[] = { "appmenu", NULL};
static const char *pavucontrol[] = { "pavucontrol", NULL};
static const char *instantsettings[] = { "instantsettings", NULL};
static const char  *clickcmd[] = { "autoclicker", NULL };
static const char  *codecmd[] = { "code", NULL };
static const char  *startmenucmd[] = { "instantstartmenu", NULL };

static const char  *scrotcmd[] = { "/opt/instantos/menus/dm/ss.sh", NULL };
static const char  *fscrotcmd[] = { "/opt/instantos/menus/dm/sm.sh", NULL };
static const char  *clipscrotcmd[] = { "/opt/instantos/menus/dm/sc.sh", NULL };
static const char  *fclipscrotcmd[] = { "/opt/instantos/menus/dm/sf.sh", NULL };

static const char  *firefoxcmd[] = { "firefox", NULL };

static const char *spoticli[] = { "spoticli", "m", NULL};
static const char *spotiprev[] = { "spoticli", "p", NULL};
static const char *spotinext[] = { "spoticli", "n", NULL};

#include "push.c"

ResourcePref resources[] = {
		{ "bgcolor",        STRING,  &col_background },
		{ "fontcolor",        STRING,  &col_white },
		{ "selectcolor",        STRING,  &col_blue },
		{ "closecolor",        STRING,  &col_red },
};

static Key dkeys[] = {
	/* modifier                     key        function        argument */
	{0, XK_r, spawn, {.v = rangercmd } },
	{0, XK_n, spawn, {.v = nautiluscmd } },
	{0, XK_space, spawn, {.v = panther} },
	{0, XK_f, spawn, {.v = firefoxcmd} },
	{0, XK_F1, spawn, {.v = helpcmd} },
	{0, XK_m, spawn, {.v = spoticli} },
	{0, XK_Return, spawn, {.v = termcmd} },
	{0, XK_plus, spawn, {.v = upvol} },
	{0, XK_minus, spawn, {.v = downvol} },
	{0, XK_Tab, spawn, {.v = caretinstantswitchcmd} },
	{0, XK_c, spawn, {.v = codecmd} },
	{0, XK_y, spawn, {.v = roficmd} },

	{0, XK_h,   viewtoleft,     {0}},
	{0, XK_l,  viewtoright,    {0}},
	{0, XK_k,      shiftview,      {.i = +1 } },
	{0, XK_j,      shiftview,      {.i = -1 } },

	{0, XK_Left,   viewtoleft,     {0}},
	{0, XK_Right,  viewtoright,    {0}},
	{0, XK_Up,      shiftview,      {.i = +1 } },
	{0, XK_Down,      shiftview,      {.i = -1 } },

	{0, XK_1, view, {.ui = 1 << 0}},
	{0, XK_2, view, {.ui = 1 << 1}},
	{0, XK_3, view, {.ui = 1 << 2}},
	{0, XK_4, view, {.ui = 1 << 3}},
	{0, XK_5, view, {.ui = 1 << 4}},
	{0, XK_6, view, {.ui = 1 << 5}},
	{0, XK_7, view, {.ui = 1 << 6}},
	{0, XK_8, view, {.ui = 1 << 7}},
	{0, XK_9, view, {.ui = 1 << 8}},

};

static Key keys[] = {
	/* modifier                     key        function        argument */

	{MODKEY|Mod1Mask,				XK_j,  	keyresize,	{.i = 0}},
	{MODKEY|Mod1Mask,				XK_k,  	keyresize,	{.i = 1}},
	{MODKEY|Mod1Mask,				XK_l,  	keyresize,	{.i = 2}},
	{MODKEY|Mod1Mask,				XK_h,  	keyresize,	{.i = 3}},
	{MODKEY|ControlMask,			XK_d,  	distributeclients,	{0}},
	{MODKEY|ShiftMask,				XK_d,  	drawwindow,	{0}},

	{MODKEY, XK_r, spawn, {.v = rangercmd } },
	{MODKEY|ControlMask|Mod1Mask, XK_r, redrawwin, {0} },
	{MODKEY, XK_n, spawn, {.v = nautiluscmd } },
	{MODKEY | ControlMask, XK_q, spawn, {.v = instantshutdowncmd } },
	{MODKEY, XK_y, spawn, {.v = panther} },
	{MODKEY, XK_a, spawn, {.v = instantassistcmd} },
	{MODKEY, XK_w, setoverlay, {0} },
	{MODKEY | ControlMask, XK_w, createoverlay, {0} },
	{MODKEY, XK_g, spawn, {.v = notifycmd} },
	{MODKEY | ControlMask, XK_space, spawn, {.v = instantmenucmd}},
	{MODKEY, XK_space, spawn, {.v = roficmd}},
	{MODKEY, XK_minus, spawn, {.v = instantmenustcmd}},
	{MODKEY, XK_x, spawn, {.v = instantswitchcmd}},
	{Mod1Mask, XK_Tab, spawn, {.v = iswitchcmd}},
	{MODKEY, XK_dead_circumflex, spawn, {.v = caretinstantswitchcmd}},
	{MODKEY | ControlMask, XK_l, spawn, {.v = slockcmd}},
	{MODKEY | ControlMask, XK_h, hidewin, {0}},
	{MODKEY | Mod1Mask | ControlMask, XK_h, unhideall, {0}},
	{MODKEY | Mod1Mask | ControlMask, XK_l, spawn, {.v = slockmcmd}},
	{MODKEY, XK_Return, spawn, {.v = termcmd}},
	{MODKEY, XK_v, spawn, {.v = quickmenucmd}},
	{MODKEY, XK_b, togglebar, {0}},
	{MODKEY, XK_j, focusstack, {.i = +1}},
	{MODKEY, XK_Down, focusstack, {.i = +1}},
	{MODKEY, XK_k, focusstack, {.i = -1}},
	{MODKEY, XK_Up, focusstack, {.i = -1}},
	{MODKEY|ControlMask, XK_j, pushdown, {0} },
	{MODKEY|ControlMask, XK_k, pushup, {0} },
	{MODKEY|Mod1Mask, XK_s, togglealttag, {0} },
	{MODKEY|ShiftMask|Mod1Mask, XK_s, toggleanimated, {0} },
	{MODKEY,                    XK_s,      togglesticky,   {0} },
	{MODKEY|ShiftMask, XK_f, togglefakefullscreen, {0} },
	{MODKEY | ShiftMask | Mod1Mask, XK_d, toggledoubledraw, {0} },
	{MODKEY|ShiftMask, XK_w, warpfocus, {0} },
	{MODKEY|Mod1Mask, XK_w, centerwindow, {0} },
	{MODKEY|ShiftMask, XK_s, toggleshowtags, {0} },
	{MODKEY, XK_i, incnmaster, {.i = +1}},
	{MODKEY, XK_d, incnmaster, {.i = -1}},
	{MODKEY, XK_h, setmfact, {.f = -0.05}},
	{MODKEY, XK_l, setmfact, {.f = +0.05}},
	{MODKEY | ShiftMask, XK_Return, zoom, {0}},
	{MODKEY, XK_Tab, view, {0}},
	{MODKEY|ShiftMask, XK_Tab, focuslastclient, {0}},
	{MODKEY|Mod1Mask, XK_Tab, followview, {0}},
	{MODKEY, XK_q, shutkill, {0}},
	{MODKEY|ShiftMask, XK_c, killclient, {0}},
	{Mod1Mask, XK_F4, killclient, {0}},
	{MODKEY, XK_F1, spawn, {.v = helpcmd}},
	{MODKEY, XK_F2, toggleprefix, {0}},
	{MODKEY, XK_t, setlayout, {.v = &layouts[0]}},
	{MODKEY, XK_f, setlayout, {.v = &layouts[2]}},
	{MODKEY, XK_m, setlayout, {.v = &layouts[3]}},
	{MODKEY|ShiftMask, XK_m, movemouse, {0}},
	{MODKEY|Mod1Mask, XK_m, resizemouse, {0}},
	{MODKEY, XK_c, setlayout, {.v = &layouts[1]}},

	{MODKEY,                       XK_Left,   animleft,     {0}},
	{MODKEY,                       XK_Right,  animright,    {0}},

	{MODKEY,                       XK_e,  overtoggle,    {.ui = ~0}},
	{MODKEY|ShiftMask,             XK_e,  fullovertoggle,    {.ui = ~0}},

	{MODKEY|ControlMask,           XK_Left,   shiftview,      {.i = -1 }},
	{MODKEY|Mod1Mask,              XK_Left,   moveleft,     {0}},
	{MODKEY|ControlMask,           XK_Right,  shiftview,      {.i = +1 }},

	{MODKEY|Mod1Mask,              XK_Right,  moveright,     {0}},

	{MODKEY|ShiftMask,             XK_Left,   tagtoleft,      {0}},
	{MODKEY|ShiftMask,             XK_Right,  tagtoright,     {0}},

	{MODKEY|ShiftMask,				XK_j,  	moveresize,	{.i = 0}},
	{MODKEY|ShiftMask,				XK_k,  	moveresize,	{.i = 1}},
	{MODKEY|ShiftMask,				XK_l,  	moveresize,	{.i = 2}},
	{MODKEY|ShiftMask,				XK_h,  	moveresize,	{.i = 3}},


	{MODKEY|ControlMask,		XK_comma,  cyclelayout,    {.i = -1 } },
	{MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },
	{MODKEY, XK_p, setlayout, {0}},
	{MODKEY | ShiftMask, XK_space, togglefloating, {0}},
	{MODKEY, XK_0, view, {.ui = ~0}},
	{MODKEY | ShiftMask, XK_0, tag, {.ui = ~0}},
	{MODKEY, XK_comma, focusmon, {.i = -1}},
	{MODKEY, XK_period, focusmon, {.i = +1}},
	{MODKEY | ShiftMask, XK_comma, tagmon, {.i = -1}},
	{MODKEY | ShiftMask, XK_period, tagmon, {.i = +1}},
	{MODKEY | Mod1Mask, XK_comma, followmon, {.i = -1}},
	{MODKEY | Mod1Mask, XK_period, followmon, {.i = +1}},

	{MODKEY | ShiftMask | ControlMask | Mod1Mask, XK_period, desktopset, {0}},
	TAGKEYS(XK_1, 0)
	TAGKEYS(XK_2, 1)
	TAGKEYS(XK_3, 2)
	TAGKEYS(XK_4, 3)
	TAGKEYS(XK_5, 4)
	TAGKEYS(XK_6, 5)
	TAGKEYS(XK_7, 6)
	TAGKEYS(XK_8, 7)
	TAGKEYS(XK_9, 8){MODKEY | ShiftMask | ControlMask, XK_q, quit, {0}},
	{0, XF86XK_AudioLowerVolume, spawn, {.v = downvol}},
	{0, XF86XK_AudioMute, spawn, {.v = mutevol}},
	{0, XF86XK_AudioRaiseVolume, spawn, {.v = upvol}},
	{0, XF86XK_AudioPlay, spawn, {.v = spoticli}},
	{0, XF86XK_AudioNext, spawn, {.v = spotinext}},
	{0, XF86XK_AudioPrev, spawn, {.v = spotiprev}},

	{0, XK_Print, spawn, {.v = fscrotcmd}},
	{MODKEY, XK_Print, spawn, {.v = scrotcmd}},
	{MODKEY|ControlMask, XK_Print, spawn, {.v = clipscrotcmd}},
	{MODKEY|Mod1Mask, XK_Print, spawn, {.v = fclipscrotcmd}},

	{ MODKEY, XK_o, winview, {0} },

};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        cyclelayout,    {.i = -1 } },
	{ ClkLtSymbol,          0,              Button3,        cyclelayout,    {.i = +1 } },
	{ ClkLtSymbol,          MODKEY,         Button1,        createoverlay,  {0} },
	{ ClkLtSymbol,          0,              Button2,        setlayout,      {.v = &layouts[0]} },
	{ ClkWinTitle,          0,              Button1,        dragmouse,      {0} },
	{ ClkWinTitle,          MODKEY,         Button1,        setoverlay,     {0} },
	{ ClkWinTitle,          MODKEY,         Button3,        spawn,          {.v = notifycmd } },
	{ ClkStatusText,        0,              Button3,        spawn,          {.v = caretinstantswitchcmd } },
	{ ClkWinTitle,          0,              Button2,        closewin,       {0} },
	{ ClkCloseButton,       0,              Button1,        killclient,     {0} },
	{ ClkCloseButton,       0,              Button3,        togglelocked,   {0} },
	{ ClkWinTitle,          0,              Button3,        dragrightmouse, {0} },
	{ ClkWinTitle,          0,              Button5,        focusstack,     {.i = +1} },
	{ ClkWinTitle,          0,              Button4,        focusstack,     {.i = -1} },
	{ ClkWinTitle,          ShiftMask,      Button5,        pushdown,       {0} },
	{ ClkWinTitle,          ShiftMask,      Button4,        pushup,         {0} },
	{ ClkWinTitle,          ControlMask,    Button5,        downscaleclient,{0} },
	{ ClkWinTitle,          ControlMask,    Button4,        upscaleclient,  {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkStatusText,        0,              Button4,        spawn,          {.v = upvol } },
	{ ClkStatusText,        0,              Button5,        spawn,          {.v = downvol } },
	{ ClkStatusText,        MODKEY,         Button2,        spawn,          {.v = mutevol } },
	{ ClkStatusText,        0,              Button1,        spawn,          {.v = panther } },
	{ ClkStatusText,        MODKEY|ShiftMask,Button1,       spawn,          {.v = pavucontrol } },
	{ ClkStatusText,        MODKEY,         Button1,        spawn,           {.v = instantsettings } },
	{ ClkStatusText,        MODKEY,         Button3,        spawn,          {.v = spoticli } },
	{ ClkStatusText,        MODKEY,         Button4,        spawn,          {.v = upbright } },
	{ ClkStatusText,        MODKEY,         Button5,        spawn,          {.v = downbright } },
	{ ClkRootWin,           MODKEY,         Button3,        spawn,          {.v = notifycmd } },
	{ ClkRootWin,           0,              Button1,        spawn,          {.v = panther } },
	{ ClkRootWin,           MODKEY,         Button1,        setoverlay,     {0} },
	{ ClkRootWin,           0,              Button3,        spawn,          {.v = roficmd } },
	{ ClkRootWin,           0,              Button2,        spawn,          {.v = instantmenucmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkClientWin,         MODKEY|Mod1Mask,Button3,        forceresizemouse,    {0} },
	{ ClkClientWin,         MODKEY | ShiftMask,             Button3,        resizeaspectmouse,    {0} },
	{ ClkTagBar,            0,              Button1,        dragtag,        {0} },
	{ ClkTagBar,            0,              Button5,        viewtoright,    {0} },
	{ ClkTagBar,            MODKEY,         Button4,        shiftview,      {.i = -1 } },
	{ ClkTagBar,            MODKEY,         Button5,        shiftview,      {.i = +1 } },
	{ ClkTagBar,            0,              Button4,        viewtoleft,     {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            Mod1Mask,       Button1,        followtag,      {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkShutDown,          0,              Button1,        spawn,          {.v = instantshutdowncmd } },
	{ ClkShutDown,          0,              Button3,        spawn,          {.v = slockcmd } },
	{ ClkShutDown,          0,              Button2,        spawn,          {.v = oslockcmd } },
	{ ClkSideBar,           0,              Button1,        gesturemouse,   {0} },
	{ ClkStartMenu,         0,              Button1,        spawn,          {.v = startmenucmd}},
	{ ClkStartMenu,         ShiftMask,      Button1,        toggleprefix,   {0}},
	{ ClkStartMenu,         0,              Button3,        spawn,          {.v = quickmenucmd}},
};
