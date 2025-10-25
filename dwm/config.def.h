/* DWM config by Nicklas Rudolfsson - https://github.com/nirucon */
/* Vanilla base (dwm 6.6). Requires: brightnessctl, PipeWire, maim, flameshot, rofi, st, alacritty, brave, arandr, playerctl */

/* ---- Compatibility shim for some patched dwm.c expecting `refreshrate` ---- */
#ifndef REFRESH_RATE
#define REFRESH_RATE 60
#endif
#ifndef refreshrate
#define refreshrate REFRESH_RATE
#endif
/* ---- end shim ---- */

#include <X11/XF86keysym.h> /* hardware keys */

/*========================
  Appearance / Colors
  "MatteBlack" theme
=========================*/
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 15;       /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 hides bar */
static const int topbar             = 1;        /* 0 bottom bar */

static const char *fonts[]          = {
	"JetBrainsMono Nerd Font:size=11",
	"monospace:size=11"
};
static const char dmenufont[]       = "JetBrainsMono Nerd Font:size=11";

/* MatteBlack palette */
static const char col_bg[]          = "#0f0f10";  /* background */
static const char col_fg[]          = "#e5e5e5";  /* foreground */
static const char col_fg_dim[]      = "#a8a8a8";  /* dim/inactive text */
static const char col_accent[]      = "#3a3a3d";  /* selection bg */
static const char col_border[]      = "#2a2a2d";  /* border (normal) */
static const char col_border_sel[]  = "#5a5a60";  /* border (selected) */

static const char *colors[][3]      = {
	/*               fg         bg           border          */
	[SchemeNorm] = { col_fg_dim, col_bg,      col_border     },
	[SchemeSel]  = { col_fg,     col_accent,  col_border_sel },
};

/*========================
  Tagging / Rules
=========================*/
static const char *tags[] = { "1","2","3","4","5","6","7","8","9" };

/* Keep rules minimal/vanilla; no app auto-tagging needed for your asks */
static const Rule rules[] = {
	/* class      instance  title       tags mask  isfloating  monitor */
	{ "Gimp",     NULL,     NULL,       0,         1,          -1 },
};

/* === Custom layout funcs === */
#include "tileleft.c"
#include "rstack.c"

/*========================
  Layout(s)
=========================*/
static const float mfact     = 0.55; /* master size factor [0.05..0.95] */
static const int nmaster     = 1;    /* clients in master area */
static const int resizehints = 0;    /* respect size hints */
static const int lockfullscreen = 1; /* focus on fullscreen window */

static const Layout layouts[] = {
    { "=[]",      tileleft },                  /* [0] leftstack (stack vänster, master höger) — default */
    { "[]=",      rstack },                    /* [1] rightstack (original: master vänster, stack höger) */
    { "><>",      NULL },                      /* [2] floating */
    { "[M]",      monocle },                   /* [3] monocle */
    { "|M|",      centeredmaster },            /* [4] centered master */
    { ">M>",      centeredfloatingmaster },    /* [5] centered floating master */
};

/*========================
  Key definitions & helpers
=========================*/
#define MODKEY Mod4Mask  /* Super/Win */
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/*========================
  Commands
=========================*/
/* dmenu themed to match MatteBlack */
static char dmenumon[2] = "0";
static const char *dmenucmd[] = {
	"dmenu_run",
	"-m", dmenumon,
	"-fn", dmenufont,
	"-nb", col_bg, "-nf", col_fg_dim,
	"-sb", col_accent, "-sf", col_fg,
	NULL
};

/* Scratchpad */
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };

/* Launchers / apps */
static const char *stcmd[]        = { "st", NULL };
static const char *alacrittycmd[] = { "alacritty", NULL };
static const char *bravecmd[]     = { "brave", NULL };
static const char *wallnext[]     = { "/bin/sh", "-c", "~/.local/bin/wallrotate.sh next", NULL };

/* Rofi (Run mode) */
static const char *rofiruncmd[]   = { "rofi", "-show", "run", NULL };

/* Keybindings viewer (TUI) */
static const char *keybindingscmd[] = { 
  "alacritty", "-e", "sh", "-c", 
  "~/.local/bin/dwm-keybindings.sh -i || dwm-keybindings.sh -i || echo 'Install dwm-keybindings.sh first'; read", 
  NULL 
};

/* System / security */
static const char *lockcmd[]      = { "slock", NULL };     /* Super+Esc */
static const char *fmcmd[]        = { "pcmanfm", NULL };   /* Super+Shift+f */

/* Display configuration (F1 screen button) */
static const char *arandrcmd[] = { "arandr", NULL };

/* Brightness (requires: brightnessctl) */
static const char *br_up[]   = { "brightnessctl", "set", "+5%", NULL };
static const char *br_down[] = { "brightnessctl", "set", "5%-", NULL };

/* Audio (PipeWire native) */
static const char *vol_up[]     = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@",   "5%+",    NULL };
static const char *vol_down[]   = { "wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@",   "5%-",    NULL };
static const char *vol_toggle[] = { "wpctl", "set-mute",   "@DEFAULT_AUDIO_SINK@",   "toggle", NULL };
static const char *mic_toggle[] = { "wpctl", "set-mute",   "@DEFAULT_AUDIO_SOURCE@", "toggle", NULL };

/* Media control (requires: playerctl) */
static const char *media_play[]     = { "playerctl", "play-pause", NULL };
static const char *media_next[]     = { "playerctl", "next", NULL };
static const char *media_prev[]     = { "playerctl", "previous", NULL };

/* Screenshots (requires: maim, flameshot, slop, xclip, libnotify) */
static const char *ss_select[] = {
  "sh", "-c",
  "tmp=$(mktemp /tmp/shot-XXXXXX.png); "
  "maim -s \"$tmp\" && xclip -selection clipboard -t image/png -i \"$tmp\" && "
  "notify-send 'Screenshot' 'Region copied to clipboard' && rm -f \"$tmp\"",
  NULL
};

static const char *ss_full[] = {
  "sh", "-c",
  "dir=\"$HOME/Pictures/Screenshots\"; mkdir -p \"$dir\"; "
  "file=\"$dir/$(date +%F_%H-%M-%S).png\"; "
  "maim -u \"$file\" && xclip -selection clipboard -t image/png -i \"$file\" && "
  "notify-send 'Screenshot' \"Saved & copied: $file\"",
  NULL
};

static const char *ss_flameshot[] = {
  "sh", "-c",
  "dir=\"$HOME/Pictures/Screenshots\"; mkdir -p \"$dir\"; "
  "flameshot gui -c -p \"$dir\" && notify-send 'Screenshot' 'Flameshot copied to clipboard'",
  NULL
};

/*========================
  Keys
=========================*/
static const Key keys[] = {
	/* modifier                     key                       function        argument */

	/* Launchers */
	{ MODKEY,                       XK_Return,                spawn,          {.v = stcmd } },          /* Super+Enter -> st */
	{ MODKEY|ShiftMask,             XK_Return,                spawn,          {.v = alacrittycmd } },   /* Super+Shift+Enter -> alacritty */
	{ MODKEY,                       XK_b,                     spawn,          {.v = bravecmd } },       /* Super+b -> Brave */
	{ MODKEY|ShiftMask,             XK_p,                     spawn,          {.v = rofiruncmd } },     /* Super+Shift+p -> rofi -show run */
	{ MODKEY,                       XK_p,                     spawn,          {.v = dmenucmd } },       /* Super+p -> dmenu (MatteBlack) */
	{ MODKEY|Mod1Mask,              XK_k,                     spawn,          {.v = keybindingscmd } }, /* Super+Alt+k -> keybindings viewer */
	{ MODKEY|ShiftMask,             XK_f,                     spawn,          {.v = fmcmd } },          /* Super+Shift+f -> pcmanfm */

	/* System / security */
	{ MODKEY,                       XK_Escape,                spawn,          {.v = lockcmd } },             /* Super+Esc -> slock */
	{ MODKEY|ShiftMask,             XK_Escape,                spawn,          SHCMD("systemctl suspend") },  /* Super+Shift+Esc -> suspend */

	/* Window / layout control */
	{ MODKEY,                       XK_j,                     focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,                     focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,                     incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,                     incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,                     setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,                     setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Tab,                   view,           {0} },
	{ MODKEY,                       XK_space,                 setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,                 togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_j,                     rotatestack,    {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,                     rotatestack,    {.i = -1 } },

	/* Layout selection */
	{ MODKEY,                       XK_t,                     setlayout,      {.v = &layouts[0]} },    /* Super+t -> leftstack */
	{ MODKEY,                       XK_y,                     setlayout,      {.v = &layouts[1]} },    /* Super+y -> rightstack */
	{ MODKEY,                       XK_f,                     setlayout,      {.v = &layouts[2]} },    /* Super+f -> floating */
	{ MODKEY,                       XK_m,                     setlayout,      {.v = &layouts[3]} },    /* Super+m -> monocle */
	{ MODKEY,                       XK_c,                     setlayout,      {.v = &layouts[4]} },    /* Super+c -> centered master */
	{ MODKEY,                       XK_u,                     setlayout,      {.v = &layouts[5]} },    /* Super+u -> centered floating master */

	/* Monitors */
	{ MODKEY,                       XK_comma,                 focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,                focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,                 tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,                tagmon,         {.i = +1 } },

	/* Tags */
	TAGKEYS(                        XK_1,                                     0)
	TAGKEYS(                        XK_2,                                     1)
	TAGKEYS(                        XK_3,                                     2)
	TAGKEYS(                        XK_4,                                     3)
	TAGKEYS(                        XK_5,                                     4)
	TAGKEYS(                        XK_6,                                     5)
	TAGKEYS(                        XK_7,                                     6)
	TAGKEYS(                        XK_8,                                     7)
	TAGKEYS(                        XK_9,                                     8)
	{ MODKEY,                       XK_0,                     view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,                     tag,            {.ui = ~0 } },

	/* Bar toggle */
	{ MODKEY|ShiftMask,             XK_b,                     togglebar,      {0} },

	/* Kill active window & restart dwm */
	{ MODKEY,                       XK_q,                     killclient,     {0} },          /* Super+q -> kill focused */
	{ MODKEY|ShiftMask,             XK_q,                     quit,           {0} },          /* Super+Shift+q -> quit */

	/* Zoom (vanilla) */
	{ MODKEY|ShiftMask,             XK_z,                     zoom,           {0} },

	/* Scratchpad */
	{ MODKEY|ShiftMask,             XK_s,                     togglescratch,  {.v = scratchpadcmd } },

	/* Hardware keys - Brightness */
	{ 0,                            XF86XK_MonBrightnessUp,   spawn,          {.v = br_up } },
	{ 0,                            XF86XK_MonBrightnessDown, spawn,          {.v = br_down } },

	/* Hardware keys - Audio */
	{ 0,                            XF86XK_AudioRaiseVolume,  spawn,          {.v = vol_up } },
	{ 0,                            XF86XK_AudioLowerVolume,  spawn,          {.v = vol_down } },
	{ 0,                            XF86XK_AudioMute,         spawn,          {.v = vol_toggle } },
	{ 0,                            XF86XK_AudioMicMute,      spawn,          {.v = mic_toggle } },

	/* Hardware keys - Media control */
	{ 0,                            XF86XK_AudioPlay,         spawn,          {.v = media_play } },    /* Play/Pause */
	{ 0,                            XF86XK_AudioPause,        spawn,          {.v = media_play } },    /* Pause (same as play) */
	{ 0,                            XF86XK_AudioNext,         spawn,          {.v = media_next } },    /* Next track */
	{ 0,                            XF86XK_AudioPrev,         spawn,          {.v = media_prev } },    /* Previous track */

	/* Hardware keys - Display & AI */
	{ 0,                            XF86XK_Display,           spawn,          {.v = arandrcmd } },          /* F1/Display -> arandr */

	/* Wallpaper */
	{ MODKEY|ShiftMask,             XK_w,                     spawn,          {.v = wallnext } },      /* Super+Shift+W -> next wallpaper */

	/* Screenshots */
	{ 0,                            XK_Print,                 spawn,          {.v = ss_select } },     /* Print -> maim -s (region) */
	{ MODKEY,                       XK_Print,                 spawn,          {.v = ss_full } },       /* Super+Print -> full screen */
	{ MODKEY|ShiftMask,             XK_Print,                 spawn,          {.v = ss_flameshot } },  /* Super+Shift+Print -> flameshot gui */
};

/*========================
  Mouse buttons
=========================*/
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = stcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
