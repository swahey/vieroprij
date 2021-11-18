
 // compile: gcc -Wall -Wextra -o VierOpRij VierOpRij.c $(pkg-config gtk+-3.0 --cflags --libs)

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


GtkWidget *window;
GtkWidget *draw_labels;
GtkWidget *draw_matrix;
GtkWidget *grid;

GtkWidget *nieuwSpel;
GtkWidget *resetScore;
GtkWidget *wButton[9];    // gooi knoppen

gboolean prn = FALSE;     // zet naar "TRUE" om alle bewerkingen te tonen in therminal

gboolean kolnumgebruiken[9];
gboolean jijWint = FALSE;
gboolean ikSpeel = FALSE;
gboolean VierOpEenRij = FALSE;
gboolean VierOpEenRijComputer = FALSE;
gboolean boolSetScore = TRUE;
gboolean BordVol = FALSE;
gboolean bordLeeg = TRUE;

// berekening Factor grote: 8 rijen x Factor = 144 dus, Factor is 18 voor een breedte van 144
// door Factor aan te passen vergroot of verkleint het speelveld
int Factor = 18;

char *tekstUwScore = "0";
char *tekstMijnScore = "0";
char *tekstInfo = "VIER OP RIJ";
char *tekstLaatsteZet = "Laatste Zet: R? K?";

GDateTime *date_time;
gchar *strTime;
gchar *strKlokDatum;
int aantalZetten = 0;
int pulsTeller = 0;
int kolomNummer = 0;
int veld[7][9];
int mijnScoreTeller = 0;
int uwScoreTeller = 0;
int rndgetal = 1;
int x_random = 0;
gboolean random_ok = FALSE;

// animated worp:
int anim_r = 1;
int anim_k = 1;
int anim_s = 1;
int anim_max = 6;
gboolean anim = FALSE;

// bord animated leegmaken
gboolean anim_nieuw_spel = FALSE;
int anim_nieuw_spel_teller = 0;

// declaretions:
gchar *str(int x);
void reset_score();
void nieuw_spel();
void zet_kleur(int rij, int kol);
void zet_vier_op_rij_kleur(int winnendspeler);
void zet_speler();
void verwerk_buttonkeuze(int b);
void disable_buttons();
void enable_buttons();
int zet_voor_winnen(int speler);
int computer_zet_voor_drie_op_rij_om_te_winnen();
int computer_zet_voor_drie_op_rij(int pog);
int computer_zet_tegen_speler();
int check_onderliggend_veld(int r, int k);
int computer_zet_tegen_twee_naast_elkaar(int pog);
int computer_zet_tegen_twee_naast_elkaar_met_een_tussen();
int computer_zet_in_buurt_van_speler(int pog);
gboolean check_vier_op_rij(int speler);
gboolean check_random_zet( int x_rand );
gboolean check_bord_vol();
gboolean get_Tijd();
gboolean draw_labels_callback (GtkWidget *widget, cairo_t *cr, gpointer data);
gboolean draw_matrix_callback (GtkWidget *widget, cairo_t *cr, gpointer data);


gchar *str(int x)
{
    gchar *y = g_strdup_printf("%i", x);        //  maak een string van en integer
    return y;
}

void reset_score()
{
    uwScoreTeller = 0;
    tekstUwScore = "0";
    mijnScoreTeller = 0;
    tekstMijnScore = "0";

    if (prn) { prn = FALSE; }else{ prn = TRUE; }
}

void nieuw_spel()
{
    anim_nieuw_spel_teller = 0;
    anim_nieuw_spel = TRUE;

    enable_buttons();
    if (!bordLeeg)
        disable_buttons();

    if (jijWint)
        ikSpeel = TRUE;
    zet_speler();
    jijWint = FALSE;
    VierOpEenRij = FALSE;
    VierOpEenRijComputer = FALSE;
    boolSetScore = TRUE;
    BordVol = FALSE;
    bordLeeg = TRUE;
    x_random = 0;
    aantalZetten = 0;
    tekstLaatsteZet = "Laatste Zet: R? K?";
    for (int x=1; x<9; x++)
    {
        kolnumgebruiken[x] = TRUE;
    }
}

void zet_speler()
{
    if (ikSpeel)
    {
        tekstInfo = "Mijn Beurt";
    }
    else
    {
        enable_buttons();
        tekstInfo = "Uw Beurt";
        bordLeeg = FALSE;
    }
}

void verwerk_buttonkeuze(int k)
{
    disable_buttons();

    // animated worp:
    anim_r = 1;
    anim_k = k;
    anim_s = 1;
    if (ikSpeel)
        anim_s = 2;
    for (int x = 6; x > 0; x--)
    {
        if (veld[x][k] == 0)
        {
            anim_max = x;
            anim = TRUE;
            break;
        }
    }

    aantalZetten++;

    for (int i=6; i>0; i--)
    {
        if (veld[i][k] == 0)
        {
            if (ikSpeel)
            {
                ikSpeel = FALSE;
                if (prn){ g_print("Zet Nr %i  veld %i %i = 2\n", aantalZetten, i, k); } // TEST
                tekstLaatsteZet = g_strconcat("Laatste Zet: R", str(i), " K", str(k), NULL);
            }
            else
            {
                ikSpeel = TRUE;
                if (prn){ g_print("Zet Nr %i  veld %i %i = 1\n", aantalZetten, i, k); } // TEST
                tekstInfo = "Ik Denk";
                tekstLaatsteZet = g_strconcat("Laatste Zet: R", str(i), " K", str(k), NULL);
            }
            break;
        }
    }
    pulsTeller = 0;
}

void button_gedrukt(int x)
{
    x_random = 0;
    if (prn){ g_print("BUTTON %i\n", x); } // TEST
    verwerk_buttonkeuze(x);
}

void wbutton1()
{
    button_gedrukt(1);
}

void wbutton2()
{
    button_gedrukt(2);
}

void wbutton3()
{
    button_gedrukt(3);
}

void wbutton4()
{
    button_gedrukt(4);
}

void wbutton5()
{
    button_gedrukt(5);
}

void wbutton6()
{
    button_gedrukt(6);
}

void wbutton7()
{
    button_gedrukt(7);
}

void wbutton8()
{
    button_gedrukt(8);
}

int check_onderliggend_veld(int r, int k)
{
    int rijNr = r;
    int kolNr = k;
    if (kolNr > 0)
    {
        if (rijNr < 6)
        {
            if (veld[rijNr + 1][kolNr] == 0)
            {
                if (prn){ g_print("rijNr %i kolNr %i  reset naar 0, onderliggend veld niet bezet !\n", rijNr, kolNr); } // TEST
                kolNr = 0;
            }
        }
    }
    return kolNr;
}

int zet_voor_winnen(int speler)
{
    int rijNum = 0;
    int kolNum = 0;
    int S = speler;
    int R, K, A, B, C, D;

    // check vertikaal:
    for (R = 1; R < 4; R++)
    {
        for (K = 1;  K < 9; K++)
        {
            A = veld[R][K];
            B = veld[R + 1][K];
            C = veld[R + 2][K];
            D = veld[R + 3][K];
            if ((A == 0) & (B == S) & (C == S) & (D == S))
            {
                if (prn){ g_print("zet_voor_winnen VER R = %i en K = %i :: A %i, B %i, C %i, D %i\n", R, K, A, B, C, D); } // TEST
                kolNum = K;
                break;
            }
        }
        if (kolNum > 0)
            break;
    }

    if (kolNum == 0)
    {
      // check horizontaal:
      for (R = 6; R > 0; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R][K + 1];
            C = veld[R][K + 2];
            D = veld[R][K + 3];
            if ((A == 0) & (B == S) & (C == S) & (D == S))    // 0111
            {
                rijNum = R;
                kolNum = K;
                if (prn){ g_print("zet_voor_winnen HOR R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum); } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == S) & (B == 0) & (C == S) & (D == S))    // 1011
            {
                rijNum = R;
                kolNum = K + 1;
                if (prn){ g_print("zet_voor_winnen HOR R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum); } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == S) & (B == S) & (C == 0) & (D == S))    // 1101
            {
                rijNum = R;
                kolNum = K + 2;
                if (prn){ g_print("zet_voor_winnen HOR R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum); } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == S) & (B == S) & (C == S) & (D == 0))    // 1110
            {
                rijNum = R;
                kolNum = K + 3;
                if (prn){ g_print("zet_voor_winnen HOR R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum); } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (kolNum > 0)
            break;
      }
    }
    
    if (kolNum == 0)
    {
      // check diagonaal "\"
      for (R = 1; R < 4; R++)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R + 1][K + 1];
            C = veld[R + 2][K + 2];
            D = veld[R + 3][K + 3];
            if ((A == 0) & (B == S) & (C == S) & (D == S))    // 0111
            {
                rijNum = R;
                kolNum = K;
                if (prn){ g_print("zet_voor_winnen Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum); } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == S) & (B == 0) & (C == S) & (D == S))    // 1011
            {
                rijNum = R + 1;
                kolNum = K + 1;
                if (prn){ g_print("zet_voor_winnen Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum); } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == S) & (B == S) & (C == 0) & (D == S))    // 1101
            {
                rijNum = R + 2;
                kolNum = K + 2;
                if (prn){ g_print("zet_voor_winnen Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum); } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == S) & (B == S) & (C == S) & (D == 0))    // 1110
            {
                rijNum = R + 3;
                kolNum = K + 3;
                if (prn){ g_print("zet_voor_winnen Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum); } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (kolNum > 0)
            break;
      }
    }

    if (kolNum == 0)
    {
      // check diagonaal "/"
      for (R = 6; R > 3; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R - 1][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 3][K + 3];
            if ((A == 0) & (B == S) & (C == S) & (D == S))    // 0111
            {
                rijNum = R;
                kolNum = K;
                if (prn){ g_print("zet_voor_winnen Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == S) & (B == 0) & (C == S) & (D == S))    // 1011
            {
                rijNum = R - 1;
                kolNum = K + 1;
                if (prn){ g_print("zet_voor_winnen Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == S) & (B == S) & (C == 0) & (D == S))    // 1101
            {
                rijNum = R - 2;
                kolNum = K + 2;
                if (prn){ g_print("zet_voor_winnen Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == S) & (B == S) & (C == S) & (D == 0))    // 1110
            {
                rijNum = R - 3;
                kolNum = K + 3;
                if (prn){ g_print("zet_voor_winnen Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (kolNum > 0)
            break;
      }
    }
    if (kolNum > 0)
    {
        if (S == 1)
        {
            if (prn){ g_print("Winning zet vermeden voor tegenspeler, kolNum %i\n", kolNum);  } // TEST
        }
        else
        {
            if (prn){ g_print("Winning zet voor computer, kolNum %i\n", kolNum);  } // TEST
        }
    }
    else
    {
        if (S == 1)
        {
            if (prn){ g_print("Geen Winning zet voor tegenspeler gevonden\n");  } // TEST
        }
        else
        {
            if (prn){ g_print("Geen Winning zet voor computer gevonden\n");  } // TEST
        }
    }

    return kolNum;
}

int computer_zet_tegen_speler()
{
    int rijNum = 0;
    int kolNum = 0;
    int R, K, A, B, C, D;

    // check vertikaal:
    for (R = 1; R < 4; R++)
    {
        for (K = 1;  K < 9; K++)
        {
            A = veld[R][K];
            B = veld[R + 1][K];
            C = veld[R + 2][K];
            D = veld[R + 3][K];
            if ((A == 0) & (B == 1) & (C == 1) & (D == 1))
            {
                if (prn){ g_print("computer_zet_tegen_speler VER R = %i en K = %i :: A %i, B %i, C %i, D %i\n", R, K, A, B, C, D);  } // TEST
                kolNum = K;
                break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }

    if (kolNum == 0)
    {
      // check horizontaal:
      for (R = 6; R > 0; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R][K + 1];
            C = veld[R][K + 2];
            D = veld[R][K + 3];
            if ((A == 0) & (B == 1) & (C == 1) & (D == 1))    // 0111
            {
                rijNum = R;
                kolNum = K;
                if (prn){ g_print("computer_zet_tegen_speler HOR R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 0) & (C == 1) & (D == 1))    // 1011
            {
                rijNum = R;
                kolNum = K + 1;
                if (prn){ g_print("computer_zet_tegen_speler HOR R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 1) & (C == 0) & (D == 1))    // 1101
            {
                rijNum = R;
                kolNum = K + 2;
                if (prn){ g_print("computer_zet_tegen_speler HOR R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 1) & (C == 1) & (D == 0))    // 1110
            {
                rijNum = R;
                kolNum = K + 3;
                if (prn){ g_print("computer_zet_tegen_speler HOR R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
      }
    }
    
    if (kolNum == 0)
    {
      // check diagonaal "\"
      for (R = 6; R > 3; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R - 3][K];
            B = veld[R - 2][K + 1];
            C = veld[R - 1][K + 2];
            D = veld[R][K + 3];
            if ((A == 0) & (B == 1) & (C == 1) & (D == 1))    // 0111
            {
                rijNum = R - 3;
                kolNum = K;
                if (prn){ g_print("computer_zet_tegen_speler DiN R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 0) & (C == 1) & (D == 1))    // 1011
            {
                rijNum = R - 2;
                kolNum = K + 1;
                if (prn){ g_print("computer_zet_tegen_speler DiN R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 1) & (C == 0) & (D == 1))    // 1101
            {
                rijNum = R - 1;
                kolNum = K + 2;
                if (prn){ g_print("computer_zet_tegen_speler DiN R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 1) & (C == 1) & (D == 0))    // 1110
            {
                rijNum = R;
                kolNum = K + 3;
                if (prn){ g_print("computer_zet_tegen_speler DiN R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }
    }

    if (kolNum == 0)
    {
    // check diagonaal "/"
    for (R = 6; R > 3; R--)
    {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R - 1][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 3][K + 3];
            if ((A == 0) & (B == 1) & (C == 1) & (D == 1))    // 0111
            {
                rijNum = R;
                kolNum = K;
                if (prn){ g_print("computer_zet_tegen_speler DiU R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 0) & (C == 1) & (D == 1))    // 1011
            {
                rijNum = R - 1;
                kolNum = K + 1;
                if (prn){ g_print("computer_zet_tegen_speler DiU R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 1) & (C == 0) & (D == 1))    // 1101
            {
                rijNum = R - 2;
                kolNum = K + 2;
                if (prn){ g_print("computer_zet_tegen_speler DiU R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 1) & (C == 1) & (D == 0))    // 1110
            {
                rijNum = R - 3;
                kolNum = K + 3;
                if (prn){ g_print("computer_zet_tegen_speler DiU R = %i en K = %i :: A %i, B %i, C %i, D %i, rijNum %i, kolNum %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                if (kolnumgebruiken[kolNum])
                    kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }
    }

    return kolNum;
}

gboolean check_vier_op_rij(int speler)
{
    gboolean vieroprij = FALSE;
    int S = speler;
    int waarde;
    if (S == 1)
        waarde = 11;    // speler
    else
        waarde = 22;    // computer

    int R, K, A, B, C, D;

    // check vertikaal:
    for (R = 1; R < 4; R++)
    {
        for (K = 1;  K < 9; K++)
        {
            A = veld[R][K];
            B = veld[R + 1][K];
            C = veld[R + 2][K];
            D = veld[R + 3][K];
            if ((A == S) & (B == S) & (C == S) & (D == S))
            {
                if (prn){ g_print("check_vier_op_rij VER R = %i en K = %i :: A %i, B %i, C %i, D %i\n", R, K, A, B, C, D);  } // TEST
                veld[R][K] = waarde;
                veld[R + 1][K] = waarde;
                veld[R + 2][K] = waarde;
                veld[R + 3][K] = waarde;
                vieroprij = TRUE;
                break;
            }
        }
        if (vieroprij)
            break;
    }

    if (!vieroprij)
    {
      // check horizontaal:
      for (R = 6; R > 0; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R][K + 1];
            C = veld[R][K + 2];
            D = veld[R][K + 3];
            if ((A == S) & (B == S) & (C == S) & (D == S))
            {
                vieroprij = TRUE;
                veld[R][K] = waarde;
                veld[R][K + 1] = waarde;
                veld[R][K + 2] = waarde;
                veld[R][K + 3] = waarde;
                if (prn){ g_print("check_vier_op_rij HOR R = %i en K = %i :: A %i, B %i, C %i, D %i\n", R, K, A, B, C, D);  } // TEST
                break;
            }
        }
        if (vieroprij)
            break;
      }
    }
    
    if (!vieroprij)
    {
      // check diagonaal "\"
      for (R = 1; R < 4; R++)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R + 1][K + 1];
            C = veld[R + 2][K + 2];
            D = veld[R + 3][K + 3];
            if ((A == S) & (B == S) & (C == S) & (D == S))
            {
                veld[R][K] = waarde;
                veld[R + 1][K + 1] = waarde;
                veld[R + 2][K + 2] = waarde;
                veld[R + 3][K + 3] = waarde;
                vieroprij = TRUE;
                if (prn){ g_print("check_vier_op_rij DiN R = %i en K = %i :: A %i, B %i, C %i, D %i\n", R, K, A, B, C, D);  } // TEST
                break;
            }
        }
        if (vieroprij)
            break;
      }
    }

    if (!vieroprij)
    {
      // check diagonaal "/"
      for (R = 6; R > 3; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R - 1][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 3][K + 3];
            if ((A == S) & (B == S) & (C == S) & (D == S))
            {
                vieroprij = TRUE;
                veld[R][K] = waarde;
                veld[R - 1][K + 1] = waarde;
                veld[R - 2][K + 2] = waarde;
                veld[R - 3][K + 3] = waarde;
                if (prn){ g_print("check_vier_op_rij DiU R = %i en K = %i :: A %i, B %i, C %i, D %i\n", R, K, A, B, C, D);  } // TEST
                break;
            }
        }
        if (vieroprij)
            break;
      }
    }

    return vieroprij;
}

int computer_zet_tegen_twee_naast_elkaar(int pog)
{
    int rijNum = 0;
    int kolNum = 0;
    int R, K, A, B, C, D, E;

    // check vertikaal:
        // niets doen

    // check horizontaal:
    for (R = 6; R > 0; R--)
    {
        for (K = 1; K < 5; K++)
        {
            A = veld[R][K];
            B = veld[R][K + 1];
            C = veld[R][K + 2];
            D = veld[R][K + 3];
            E = veld[R][K + 4];
            if ((A == 0) & (B == 1) & (C == 1) & (D == 0) & ( E == 0))  
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K + 3;
                }
                else
                {
                    rijNum = R;
                    kolNum = K;
                }
                if (prn){ g_print("01100 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i   rijNum = %i   kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 0) & (C == 1) & (D == 1) & ( E == 0))  
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K + 1;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 4;
                }
                if (prn){ g_print("00110 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i   rijNum = %i   kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((K == 1) & (A == 1) & (B == 0) & (C == 1) & (D == 0))  
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K + 1;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 3;
                }
                if (prn){ g_print("1010 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i   rijNum = %i   kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((K == 5) & (B == 0) & (C == 1) & (D == 0) & ( E == 1))  
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K + 3;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 1;
                }
                if (prn){ g_print("0101 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i   rijNum = %i   kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }
    
    if (kolNum == 0)
    {
      // check diagonaal "\"
      for (R = 6; R > 4; R--)
      {
        for (K = 1; K < 5; K++)
        {
            A = veld[R - 4][K];
            B = veld[R - 3][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 1][K + 3];
            E = veld[R][K + 4];
            if ((A == 0) & (B == 1) & (C == 1) & (D == 0) & (E == 0))
            {
                if (pog == 1)
                {
                    rijNum = R - 1;
                    kolNum = K + 3;
                }
                else
                {
                    rijNum = R - 4;
                    kolNum = K;
                }
                if (prn){ g_print("01100 Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i   rijNum = %i   kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 0) & (C == 1) & (D == 1) & (E == 0))
            {
                if (pog == 1)
                {
                    rijNum = R - 3;
                    kolNum = K + 1;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 4;
                }
                if (prn){ g_print("00110 Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i   rijNum = %i   kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((B == 0) & (C == 0) & (D == 1) & (E == 1))
            {
                rijNum = R - 2;
                kolNum = K + 2;
                if (prn){ g_print("0011 Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i   rijNum = %i   kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
      }
    }

    if (kolNum ==0)
    {
      // check diagonaal "/"
      for (R = 6; R > 4; R--)
      {
        for (K = 1; K < 5; K++)
        {
            A = veld[R][K];
            B = veld[R - 1][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 3][K + 3];
            E = veld[R - 4][K + 4];
            if ((A == 0) & (B == 1) & (C == 1) & (D == 0) & (E == 0))   
            {
                if (pog == 1)
                {
                    rijNum = R - 3;
                    kolNum = K + 3;
                }
                else
                {
                    rijNum = R;
                    kolNum = K;
                }
                if (prn){ g_print("01100 Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i   rijNum = %i   kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 1) & (C == 0) & (D == 0))   
            {
                rijNum = R - 2;
                kolNum = K + 2;
                if (prn){ g_print("1010 Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i   rijNum = %i   kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 0) & (C == 1) & (D == 1) & (E == 0))   
            {
                if (pog == 1)
                {
                    rijNum = R - 1;
                    kolNum = K + 1;
                }
                else
                {
                    rijNum = R - 4;
                    kolNum = K + 4;
                }
                if (prn){ g_print("00110 Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i   rijNum = %i   kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
      }
    }

    return kolNum;

}

int computer_zet_tegen_twee_naast_elkaar_met_een_tussen()   //  "01010"
{
    int rijNum = 0;
    int kolNum = 0;
    int R, K, A, B, C, D, E;

    // check horizontaal:
    for (R = 6; R > 0; R--)
    {
        for (K = 1; K < 5; K++)
        {
            A = veld[R][K];
            B = veld[R][K + 1];
            C = veld[R][K + 2];
            D = veld[R][K + 3];
            E = veld[R][K + 4];
            if ((A == 0) & (B == 1) & (C == 0) & (D == 1) & (E == 0))  
            {
                rijNum = R;
                kolNum = K + 2;
                if (prn){ g_print("01010 HOR R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i\n", R, K, A, B, C, D, E);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }
    
    if (kolNum == 0)
    {
      // check diagonaal "\"
      for (R = 6; R > 4; R--)
      {
        for (K = 1; K < 5; K++)
        {
            A = veld[R - 4][K];
            B = veld[R - 3][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 1][K + 3];
            E = veld[R][K + 4];
            if ((A == 0) & (B == 1) & (C == 0) & (D == 1) & (E == 0))
            {
                rijNum = R - 2;
                kolNum = K + 2;
                if (prn){ g_print("01010 DiN R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i\n", R, K, A, B, C, D, E);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
      }
    }

    if (kolNum ==0)
    {
      // check diagonaal "/"
      for (R = 6; R > 4; R--)
      {
        for (K = 1; K < 5; K++)
        {
            A = veld[R][K];
            B = veld[R - 1][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 3][K + 3];
            E = veld[R - 4][K + 4];
            if ((A == 0) & (B == 1) & (C == 0) & (D == 1) & (E == 0))   
            {
                rijNum = R - 2;
                kolNum = K + 2;
                if (prn){ g_print("01010 DiU R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i\n", R, K, A, B, C, D, E);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
      }
    }

    return kolNum;

}

int computer_zet_voor_drie_op_rij_om_te_winnen()
{
    int rijNum = 0;
    int kolNum = 0;
    int R, K, A, B, C, D, E;

    // Horizontaal
    for ( R = 6; R > 0; R-- )
    {
        for ( K = 1; K<5; K++ )
        {
            A = veld[R][K];
            B = veld[R][K + 1];
            C = veld[R][K + 2];
            D = veld[R][K + 3];
            E = veld[R][K + 4];
            if ((A == 0) & (B == 2) & (C == 2) & (D == 0) & (E == 0))    // 02200
            {
                rijNum = R;
                kolNum = K + 3;
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 0) & (C == 2) & (D == 2) & (E == 0))    // 00220
            {
                rijNum = R;
                kolNum = K + 1;
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 2) & (C == 0) & (D == 2) & (E == 0))    // 02020
            {
                rijNum = R;
                kolNum = K + 2;
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }

    if (kolNum == 0)
    {
    // Diagonaal "/"
    for ( R = 6; R > 4; R-- )
    {
        for ( K = 1; K<5; K++ )
        {
            A = veld[R][K];
            B = veld[R - 1][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 3][K + 3];
            E = veld[R - 4][K + 4];
            if ((A == 0) & (B == 2) & (C == 2) & (D == 0) & (E == 0))    // 02200
            {
                rijNum = R - 3;
                kolNum = K + 3;
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 0) & (C == 2) & (D == 2) & (E == 0))    // 00220
            {
                rijNum = R - 1;
                kolNum = K + 1;
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 2) & (C == 0) & (D == 2) & (E == 0))    // 02020
            {
                rijNum = R - 2;
                kolNum = K + 2;
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }
    }

    if (kolNum == 0)
    {
    // Diagonaal "\"
    for ( R = 6; R > 4; R-- )
    {
        for ( K = 1; K<5; K++ )
        {
            A = veld[R - 4][K];
            B = veld[R - 3][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 1][K + 3];
            E = veld[R][K + 4];
            if ((A == 0) & (B == 2) & (C == 2) & (D == 0) & (E == 0))    // 02200
            {
                rijNum = R - 1;
                kolNum = K + 3;
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 0) & (C == 2) & (D == 2) & (E == 0))    // 00220
            {
                rijNum = R - 3;
                kolNum = K + 1;
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 2) & (C == 0) & (D == 2) & (E == 0))    // 02020
            {
                rijNum = R - 2;
                kolNum = K + 2;
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }
    }

    if (kolNum > 0)
        if (prn){ g_print("computer_zet_voor_drie_op_rij_om_te_winnen   rijNum = %i   kolNum = %i\n", rijNum, kolNum);  } // TEST

    return kolNum;

}

int computer_zet_voor_drie_op_rij(int pog)
{
    int rijNum = 0;
    int kolNum = 0;
    int R, K, A, B, C, D, E;

    // Horizontaal A
    for ( R = 6; R > 0; R-- )
    {
        for ( K = 1; K<5; K++ )
        {
            A = veld[R][K];
            B = veld[R][K + 1];
            C = veld[R][K + 2];
            D = veld[R][K + 3];
            E = veld[R][K + 4];
            if ((A == 0) & (B == 2) & (C == 0) & (D == 2) & (E < 2))
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 2;
                }
                if (prn){ g_print("0202X HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((E < 2) & (B == 2) & (C == 0) & (D == 2) & (E == 0))
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K + 4;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 2;
                }
                if (prn){ g_print("X2020 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i, E %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, E, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }
            
    if (kolNum == 0)
    {
    // Horizontaal B
    for ( R = 6; R > 0; R-- )
    {
        for ( K = 1; K<6; K++ )
        {
            A = veld[R][K];
            B = veld[R][K + 1];
            C = veld[R][K + 2];
            D = veld[R][K + 3];
            if ((A == 0) & (B == 2) & (C == 2) & (D == 0))
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 3;
                }
                if (prn){ g_print("0220 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 0) & (C == 2) & (D == 2))
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 1;
                }
                if (prn){ g_print("0022 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 2) & (B == 2) & (C == 0) & (D == 0))
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K + 3;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 2;
                }
                if (prn){ g_print("2200 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 2) & (B == 0) & (C == 0) & (D == 2))
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K + 1;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 2;
                }
                if (prn){ g_print("2002 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }
    }

    if (kolNum == 0)
    {
    // Diagonaal "\"
    for ( R = 6; R > 3; R-- )
    {
        for ( K = 1; K<6; K++ )
        {
            A = veld[R - 3][K];
            B = veld[R - 2][K + 1];
            C = veld[R - 1][K + 2];
            D = veld[R][K + 3];
            if ((A == 0) & (B == 2) & (C == 2) & (D == 0))
            {
                if (pog == 1)
                {
                    rijNum = R - 3;
                    kolNum = K;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 3;
                }
                if (prn){ g_print("0220 Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 0) & (C == 2) & (D == 2))
            {
                if (pog == 1)
                {
                    rijNum = R - 2;
                    kolNum = K + 1;
                }
                else
                {
                    rijNum = R - 3;
                    kolNum = K;
                }
                if (prn){ g_print("0022 Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 2) & (B == 2) & (C == 0) & (D == 0))
            {
                if (pog == 1)
                {
                    rijNum = R - 1;
                    kolNum = K + 2;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 3;
                }
                if (prn){ g_print("2200 Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }

        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }
    }

    if (kolNum == 0)
    {
    // Diagonaal "/"
    for ( R = 6; R > 3; R-- )
    {
        for ( K = 1; K<6; K++ )
        {
            A = veld[R][K];
            B = veld[R - 1][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 3][K + 3];
            if ((A == 0) & (B == 2) & (C == 2) & (D == 0))
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K;
                }
                else
                {
                    rijNum = R - 3;
                    kolNum = K + 3;
                }
                if (prn){ g_print("0220 Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 0) & (B == 0) & (C == 2) & (D == 2))
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K;
                }
                else
                {
                    rijNum = R - 1;
                    kolNum = K + 1;
                }
                if (prn){ g_print("0022 Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 2) & (B == 2) & (C == 0) & (D == 0))
            {
                if (pog == 1)
                {
                    rijNum = R - 2;
                    kolNum = K + 2;
                }
                else
                {
                    rijNum = R - 3;
                    kolNum = K + 3;
                }
                if (prn){ g_print("2200 Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }

        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
    }
    }

    return kolNum;
}

int computer_zet_in_buurt_van_speler(int pog)
{
    int rijNum = 0;
    int kolNum = 0;
    int R, K, A, B, C, D;

    if (kolNum == 0)
    {
    // horizontaal
      for (R = 6; R > 0; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R][K + 1];
            C = veld[R][K + 2];
            D = veld[R][K + 3];
            if ((A == 0) & (B == 0) & (C == 0) & (D == 1))
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K + 1;
                }
                else
                {
                    rijNum = R;
                    kolNum = K;
                }
                if (prn){ g_print("0001 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 0) & (C == 0) & (D == 0))
            {
                if (pog == 1)
                {
                    rijNum = R;
                    kolNum = K + 2;
                }
                else
                {
                    rijNum = R;
                    kolNum = K + 3;
                }
                if (prn){ g_print("1000 HOR  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
      }
    }

    if (kolNum == 0)
    {
    // Dia U "/"
      for (R = 6; R > 3; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R - 1][K + 1];
            C = veld[R - 1][K + 2];
            D = veld[R - 1][K + 3];
            if ((A == 0) & (B == 0) & (C == 0) & (D == 1))
            {
                if (pog == 1)
                {
                    rijNum = R - 1;
                    kolNum = K + 1;
                }
                if (prn){ g_print("0001 Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 0) & (C == 0) & (D == 0))
            {
                if (pog == 1)
                {
                    rijNum = R - 2;
                    kolNum = K + 2;
                }
                if (prn){ g_print("1000 Dia U  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
      }
    }

    if (kolNum == 0)
    {
    // Dia D "\"
      for (R = 6; R > 3; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R - 3][K];
            B = veld[R - 2][K + 1];
            C = veld[R - 1][K + 2];
            D = veld[R][K + 3];
            if ((A == 0) & (B == 0) & (C == 0) & (D == 1))
            {
                if (pog == 1)
                {
                    rijNum = R - 2;
                    kolNum = K + 1;
                }
                if (prn){ g_print("0001 Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
            if ((A == 1) & (B == 0) & (C == 0) & (D == 0))
            {
                if (pog == 1)
                {
                    rijNum = R - 1;
                    kolNum = K + 2;
                }
                if (prn){ g_print("1000 Dia D  R = %i en K = %i :: A %i, B %i, C %i, D %i  rijNum = %i kolNum = %i\n", R, K, A, B, C, D, rijNum, kolNum);  } // TEST
                kolNum = check_onderliggend_veld(rijNum, kolNum);
                if (kolNum > 0)
                    break;
            }
        }
        if (!kolnumgebruiken[kolNum])
            kolNum = 0;
        if (kolNum > 0)
            break;
      }
    }

    return kolNum;
}

gboolean check_random_zet( int x_rand )
{
    int rijNum = 0;
    int kolNum = x_rand;
    int R, K, A, B, C, D;
    gboolean ran_ok = TRUE;
    if (prn){ g_print("check random zet  ::   kolNum = %i\n", kolNum); } // TEST

    if (veld[1][kolNum] != 0)
        ran_ok = FALSE;

    // check horizontaal:
    if (ran_ok)
    {
      for (R = 5; R > 0; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R][K];
            B = veld[R][K + 1];
            C = veld[R][K + 2];
            D = veld[R][K + 3];
            if ((kolNum == K) & (A == 0) & (B == 1) & (C == 1) & (D == 1))        // 0111
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", R, kolNum); } // TEST
                if ((R == 5) & (veld[R+1][K] == 0))
                {
                    ran_ok = FALSE;
                    break;
                }
                else if ((R < 5) & (veld[R+1][K] == 0) & (veld[R+2][K] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
            if ((kolNum == K + 1) & (A == 1) & (B == 0) & (C == 1) & (D == 1))    // 1011
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", rijNum, kolNum); } // TEST
                if ((R == 5) & (veld[R+1][K+1] == 0))
                {
                    ran_ok = FALSE;
                    break;
                }
                else if ((R < 5) & (veld[R+1][K+1] == 0) & (veld[R+2][K+1] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
            if ((kolNum == K + 2) & (A == 1) & (B == 1) & (C == 0) & (D == 1))    // 1101
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", rijNum, kolNum); } // TEST
                if ((R == 5) & (veld[R+1][K+2] == 0))
                {
                    ran_ok = FALSE;
                    break;
                }
                else if ((R < 5) & (veld[R+1][K+2] == 0) & (veld[R+2][K+2] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
            if ((kolNum == K + 3) & (A == 1) & (B == 1) & (C == 1) & (D == 0))    // 1110
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", rijNum, kolNum); } // TEST
                if ((R == 5) & (veld[R+1][K+3] == 0))
                {
                    ran_ok = FALSE;
                    break;
                }
                else if ((R < 5) & (veld[R+1][K+3] == 0) & (veld[R+2][K+3] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
        }
      }
    }

    // Diagonaal "/"
    if (ran_ok)
    {
      for (R = 6; R > 3; R--)
        for (K = 1; K < 6; K++)
        {
            A = veld[R ][K];
            B = veld[R - 1][K + 1];
            C = veld[R - 2][K + 2];
            D = veld[R - 3][K + 3];

            if ((K == kolNum) & (A == 0) & (B == 1) & (C == 1) & (D == 1))        // 0111
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", R, kolNum); } // TEST
                if ((R == 5) & (veld[R+1][K] == 0))
                {
                    ran_ok = FALSE;
                    break;
                }
                else if ((R < 5) & (veld[R+1][K] == 0) & (veld[R+2][K] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
            if ((K + 1 == kolNum) & (A == 1) & (B == 0) & (C == 1) & (D == 1))    // 1011
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", R, kolNum); } // TEST
                if ((R - 1 == 5) & (veld[R][K+1] == 0))
                {
                    ran_ok = FALSE;
                    break;
                }

                if ((R - 1 > 2) & (veld[R][K+1] == 0) & (veld[R+1][K+1] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
            if ((K + 2 == kolNum) & (A == 1) & (B == 1) & (C == 0) & (D == 1))    // 1101
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", R, kolNum); } // TEST
                if ((R - 2 > 1) & (veld[R-1][K+2] == 0) & (veld[R][K+2] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
            if ((K + 3 == kolNum) & (A == 1) & (B == 1) & (C == 1) & (D == 0))    // 1110
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", R, kolNum); } // TEST
                if ((R - 3 > 0) & (veld[R-2][K+3] == 0) & (veld[R-1][K+3] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
        }
    }

    // Diagonaal "\"
    if (ran_ok)
    {
      for (R = 3; R > 0; R--)
      {
        for (K = 1; K < 6; K++)
        {
            A = veld[R ][K];
            B = veld[R + 1][K + 1];
            C = veld[R + 2][K + 2];
            D = veld[R + 3][K + 3];

            if ((K == kolNum) & (A == 0) & (B == 1) & (C == 1) & (D == 1))        // 0111
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", R, kolNum); } // TEST
                if ((veld[R+1][K] == 0) & (veld[R+2][K] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
            if ((K + 1 == kolNum) & (A == 1) & (B == 0) & (C == 1) & (D == 1))    // 1011
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", R, kolNum); } // TEST
                if ((veld[R+2][K+1] == 0) & (veld[R+3][K+1] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
            if ((K + 2 == kolNum) & (A == 1) & (B == 1) & (C == 0) & (D == 1))    // 1101
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", R, kolNum); } // TEST
                if ((R < 3) & (veld[R+3][K+2] == 0) & (veld[R+4][K+2] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
            if ((K + 3 == kolNum) & (A == 1) & (B == 1) & (C == 1) & (D == 0))    // 1110
            {
                if (prn){ g_print("check random zet rijNum = %i  kolNum = %i\n", R, kolNum); } // TEST
                if ((R < 2) & (veld[R+4][K+3] == 0) & (veld[R+5][K+3] != 0))
                {
                    ran_ok = FALSE;
                    break;
                }
            }
        }
      }
    }

    return ran_ok;
}

gboolean check_bord_vol()
{
    gboolean bordV = TRUE;
    for  (int x = 1; x < 9; x++)
    {
        if (veld[1][x] == 0)
        {
            bordV = FALSE;
            break;
        }
    }
    return bordV;
}

gboolean get_Tijd()
{
    date_time = g_date_time_new_now_local();                            // get local time
    strKlokDatum = g_date_time_format(date_time, "%d:%m:%y");           // datum
    strTime = g_date_time_format(date_time, "%A %e %B %Y - %H:%M:%S");  // 24hr time format
    gtk_window_set_title (GTK_WINDOW (window), strTime);

    pulsTeller++;
    if (pulsTeller > 9)
    {
        pulsTeller = 0;
        if (!VierOpEenRij)
            VierOpEenRij = check_vier_op_rij( 1 );                                      // vier op rij voor speler ?

        if (!VierOpEenRijComputer)
            VierOpEenRijComputer = check_vier_op_rij( 2 );                              // vier op rij voor computer ?

        if (!BordVol)
            BordVol = check_bord_vol();                                                 // bord vol en geen winnaar, is gelijk spel

        if ((VierOpEenRij) | (VierOpEenRijComputer) | (BordVol))
        {
            disable_buttons();
            ikSpeel = FALSE;
            if (VierOpEenRij)
            {
                if (boolSetScore)
                {
                    boolSetScore = FALSE;
                    uwScoreTeller++;
                    tekstUwScore = str(uwScoreTeller);
                    tekstInfo = "! JIJ WINT !";
                    jijWint = TRUE;
                }
            }
            else if (VierOpEenRijComputer)
            {
                if (boolSetScore)
                {
                    boolSetScore = FALSE;
                    mijnScoreTeller++;
                    tekstMijnScore = str(mijnScoreTeller);
                    tekstInfo = "! IK WIN !";
                    jijWint = FALSE;
                }
            }
            else if (BordVol)
            {
                if (boolSetScore)
                {
                    boolSetScore = FALSE;
                    tekstInfo = "GELIJK SPEL";
                }
            }
        }
        else
        {
            zet_speler();
        }
        if ((ikSpeel) & (!anim_nieuw_spel))
        {
            usleep(300000);
            // keuze waar werpen !!!

            kolomNummer = zet_voor_winnen(2);                                           // kijken of er vier op rij kunnen voor computer
            
            if (kolomNummer == 0)
                kolomNummer = computer_zet_tegen_speler();                              // speler beletten vier op rij te maken
            
            if (kolomNummer == 0)                                 
                kolomNummer = computer_zet_tegen_twee_naast_elkaar(1);                  // zijn er twee naast elkaar zoals '0110'

            if (kolomNummer == 0)                                 
                kolomNummer = computer_zet_tegen_twee_naast_elkaar_met_een_tussen();    // zijn er twee naast elkaar zoals '01010'

            if (kolomNummer == 0)
                kolomNummer = computer_zet_voor_drie_op_rij_om_te_winnen();             // kijken of er drie op rij kunnen voor computer zodat volgende zet winnen is

            if (kolomNummer == 0)
                kolomNummer = computer_zet_voor_drie_op_rij(1);                         // kijken of er drie op rij kunnen voor computer

            if (kolomNummer == 0)
            {
                kolomNummer = rndgetal;  // random nummer
                if (prn){ g_print("random nummer = %i\n", kolomNummer); }  // TEST

                random_ok = FALSE;
                if (kolnumgebruiken[kolomNummer])
                    random_ok = check_random_zet( kolomNummer );                            // zet mag niet voordelig zijn voor tegenspeler

                if (random_ok)
                {
                    if (prn){ g_print("random_ok is TRUE   kolNum %i\n", kolomNummer); }  // TEST
                }
                else
                {
                    if (prn){ g_print("random_ok is FALSE  kolNum %i\n", kolomNummer); }  // TEST
                    while (!random_ok)
                    {
                        kolomNummer++;
                        if (kolomNummer > 8)
                            kolomNummer = 1;

                        if (kolnumgebruiken[kolomNummer])
                            random_ok = check_random_zet( kolomNummer );

                        if (random_ok)
                        {
                            if (prn){ g_print("random_ok is TRUE   x_random %i   kolNum %i\n", x_random, kolomNummer);  } // TEST
                        }
                        else
                        {
                            if (prn){ g_print("random_ok is FALSE   x_random %i   kolNum %i\n", x_random, kolomNummer);  } // TEST
                        }
                        x_random++;
                        if (x_random > 8)
                        {
                            random_ok = TRUE;
                            for (int x=1; x<9; x++)
                            {
                                if (veld[1][x] == 0)
                                {
                                    kolomNummer = x;
                                    break;
                                }
                            }
                        }
                    }
                }
            }            
            verwerk_buttonkeuze( kolomNummer );
        }
    }

    rndgetal++;
    if (rndgetal > 8)
        rndgetal = 1;
    gtk_widget_queue_draw(draw_labels);
    gtk_widget_queue_draw(draw_matrix);

    return TRUE;
}


gboolean draw_labels_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    gdouble width, height;
    gdouble x, y;
    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    // kader:
    cairo_set_line_width(cr, 2.0);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, 0,0);
    cairo_line_to(cr, width, 0);
    cairo_line_to(cr, width, height);
    cairo_line_to(cr, 0, height);
    cairo_line_to(cr, 0, 0);
    cairo_stroke(cr);

    x = 20;
    y = 5;

    // als "prn = TRUE"
    if (prn)
    {
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_font_size(cr, Factor * 0.6);
        cairo_move_to(cr, width - 30, y + height - 10);
        cairo_show_text(cr, "***");
    }

    // label Score
    cairo_set_source_rgb(cr, 0.0, 0.75, 0);
    cairo_set_font_size(cr, Factor);
    cairo_move_to(cr, width / 5 - x / 2, y + height / 4);
    cairo_show_text(cr, "SCORE");

    // mijn score
    cairo_set_source_rgb(cr, 0.75, 0, 0);
    cairo_set_font_size(cr, Factor * 1.2);
    cairo_move_to(cr, width / 4 - 2 * x, 1.5 * y + height / 4 * 3);
    cairo_show_text(cr, tekstMijnScore);
    
    // uw score
    cairo_set_source_rgb(cr, 0, 0, 0.75);
    cairo_set_font_size(cr, Factor * 1.2);
    cairo_move_to(cr, width / 4 + x, 1.5 * y + height / 4 * 3);
    cairo_show_text(cr, tekstUwScore);

    // Ik
    cairo_set_source_rgb(cr, 0.75, 0, 0);
    cairo_set_font_size(cr, Factor);
    cairo_move_to(cr, width / 4 - 4 * x, 1.5 * y + height / 4 * 3);
    cairo_show_text(cr, "Ik");

    // Jij
    cairo_set_source_rgb(cr, 0, 0, 0.75);
    cairo_set_font_size(cr, Factor);
    cairo_move_to(cr, width / 4 + 3 * x, 1.5 * y + height / 4 * 3);
    cairo_show_text(cr, "Jij");

    // score lijnen
    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgb(cr, 0, 0, 0);

    cairo_move_to(cr, width / 2, 0);
    cairo_line_to(cr, width / 2, height);
    cairo_stroke(cr);

    cairo_move_to(cr, 0, height / 2);
    cairo_line_to(cr, width / 2, height / 2);
    cairo_stroke(cr);
    
    cairo_move_to(cr, width / 4, height / 2);
    cairo_line_to(cr, width / 4, height);
    cairo_stroke(cr);



    x = width / 5 * 3;

    // laatste zet
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_font_size(cr, Factor * 0.8);
    cairo_move_to(cr, x, y + height / 4);    
    cairo_show_text(cr, tekstLaatsteZet);

    // info
    cairo_set_source_rgb(cr, 0, 0.75, 0);
    if (VierOpEenRijComputer)
        cairo_set_source_rgb(cr, 0.75, 0, 0);
    if (VierOpEenRij)
        cairo_set_source_rgb(cr, 0, 0, 0.75);
    cairo_set_font_size(cr, Factor * 1.5);
    cairo_move_to(cr, x, y + height / 4 * 3);
    if ((VierOpEenRijComputer) | (VierOpEenRij))
    {
        if (pulsTeller > 6)
        {
            cairo_set_font_size(cr, Factor * 1.6);
            cairo_move_to(cr, x - 3, y + height / 4 * 3 + 2);
        }
    }
    cairo_show_text(cr, tekstInfo);

    return FALSE;
}

gboolean draw_matrix_callback (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    gdouble width, height;
    gdouble x, y;
    gdouble A, B;
    int R, K;
    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    // kader:
    cairo_set_line_width(cr, 1.0);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, 0,0);
    cairo_line_to(cr, width, 0);
    cairo_line_to(cr, width, height);
    cairo_line_to(cr, 0, height);
    cairo_line_to(cr, 0, 0);
    cairo_stroke(cr);

    // animated worp
    if (anim)
    {
        veld[anim_r][anim_k] = anim_s;
        veld[anim_r - 1][anim_k] = 0;
        anim_r++;
        if (anim_r > anim_max)
            anim = FALSE;
    }

    // animeted bord leegmaken
    if (anim_nieuw_spel)
    {
        disable_buttons();
        int rr, kk;
        for (rr = 6; rr > 0; rr--)
        {
            for (kk = 1; kk < 9; kk++)
            {
                veld[rr][kk] = veld[rr-1][kk];
            }
        }
        anim_nieuw_spel_teller++;
        if (anim_nieuw_spel_teller > 5)
        {
            for (rr = 1; rr < 7; rr++)
            {
                for (kk = 1; kk < 9; kk++)
                {
                    veld[rr][kk] = 0;
                }
            }
            anim_nieuw_spel = FALSE;
            enable_buttons();
        }
    }

    A = width / 8;
    B = height / 6;
    R = 0;
    K = 0;
    for (y = B / 2; y < B * 6; y = y + B)
    {
        R++;
        for (x = A / 2; x < A * 8; x = x + A)
        {
            K++;
            if (K > 8)
                K = 1;
            if (veld[R][K] == 1)       // veld is bezet door speler
            {
                cairo_set_source_rgb(cr, 0.00, 0.00, 0.75);
            }
            else if (veld[R][K] == 2)  // veld is bezet door computer
            {
                cairo_set_source_rgb(cr, 0.75, 0.00, 0.00);
            }
            else if (veld[R][K] == 11)  // veld is winnend veld voor speler
            {
                if (pulsTeller < 3)
                    cairo_set_source_rgb(cr, 0.00, 0.35, 0.95);
                else
                    cairo_set_source_rgb(cr, 0.00, 0.00, 0.75);
            }
            else if (veld[R][K] == 22)  // veld is winnend veld voor computer
            {
                if (pulsTeller < 3)
                    cairo_set_source_rgb(cr, 0.95, 0.15, 0.00);
                else
                    cairo_set_source_rgb(cr, 0.75, 0.00, 0.00);
            }
            else                              // veld is nog vrij
            {
                cairo_set_source_rgb(cr, 0.99, 0.99, 0.99);
            }

            cairo_arc (cr, x, y, MIN (width, height) / ((width+height)/(Factor*3.75)), 0, 2 * G_PI);
            cairo_fill (cr);

            // kleinere gekleurde cirkel in winnende velden
            if (((veld[R][K] == 11) | (veld[R][K] == 22)) & (pulsTeller < 3))
            {
                cairo_set_source_rgb(cr, 0.55, 0.85, 0.55);
                cairo_arc (cr, x, y, MIN (width, height) / ((width+height)/(Factor*1.85)), 0, 2 * G_PI);
                cairo_fill (cr);
            }
        }
    }

    return FALSE;
}

void disable_buttons()
{
    for (int i=1; i<9; i++)
    {
        gtk_widget_set_sensitive( wButton[i], FALSE );
    }
}

void enable_buttons()
{
    for (int i=1; i<9; i++)
    {
        gtk_widget_set_sensitive( wButton[i], TRUE );
    }
}


int main(int argc, char **argv)
{

    gtk_init (&argc,&argv);

    //Declarations
    window       = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    draw_labels  = gtk_drawing_area_new();
    gtk_widget_set_size_request (draw_labels, 2*Factor*6, 2*Factor*2);
    draw_matrix  = gtk_drawing_area_new();
    gtk_widget_set_size_request (draw_matrix, 2*Factor*8, 2*Factor*6);
    grid         = gtk_grid_new ();

    //reset_score();
    
    resetScore = gtk_button_new_with_label ("Reset Score");
    nieuwSpel  = gtk_button_new_with_label ("Nieuw Spel");

    //Set Properties
    gtk_container_set_border_width (GTK_CONTAINER(window), 16);
    gtk_grid_set_row_spacing       (GTK_GRID(grid), 4);
    gtk_grid_set_column_spacing    (GTK_GRID(grid), 4);
    gtk_container_add              (GTK_CONTAINER(window), grid);


    for (int i=1; i<7; i++)
    {
        for (int j=1; j<9; j++)
        {
            veld[i][j] = 0;   // 0 is "vrij", 1 is "jij", 2 is "computer"
        }
    }

    for (int i=1; i<9; i++)
    {
        wButton[i] = gtk_button_new_with_label ("VVV");
        kolnumgebruiken[i] = TRUE;
    }


    // grid vullen                                             (x,   y,     h,     v)

    gtk_grid_attach (GTK_GRID(grid), draw_labels,               0,   0,  Factor*6, 20);
    gtk_grid_attach (GTK_GRID(grid), resetScore,         Factor*6,   0,  Factor*2, 10);
    gtk_grid_attach (GTK_GRID(grid), nieuwSpel,          Factor*6,  10,  Factor*2, 10);

    for (int i=0; i<8; i++)
    {
        gtk_grid_attach (GTK_GRID(grid), wButton[i+1],   i*Factor,  26,  Factor,   10);
    }

    gtk_grid_attach (GTK_GRID(grid), draw_matrix,               0,  36,  Factor*8, Factor*6);


    nieuw_spel();

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_timeout_add(100, (GSourceFunc) get_Tijd, NULL);  // elke xx msec functie uitvoeren

    g_signal_connect (G_OBJECT (draw_labels), "draw", G_CALLBACK (draw_labels_callback), NULL);
    g_signal_connect (G_OBJECT (draw_matrix), "draw", G_CALLBACK (draw_matrix_callback), NULL);

    g_signal_connect(G_OBJECT(nieuwSpel),  "clicked", G_CALLBACK(nieuw_spel), NULL);
    g_signal_connect(G_OBJECT(resetScore), "clicked", G_CALLBACK(reset_score), NULL);
    g_signal_connect(G_OBJECT(wButton[1]), "clicked", G_CALLBACK(wbutton1), NULL);
    g_signal_connect(G_OBJECT(wButton[2]), "clicked", G_CALLBACK(wbutton2), NULL);
    g_signal_connect(G_OBJECT(wButton[3]), "clicked", G_CALLBACK(wbutton3), NULL);
    g_signal_connect(G_OBJECT(wButton[4]), "clicked", G_CALLBACK(wbutton4), NULL);
    g_signal_connect(G_OBJECT(wButton[5]), "clicked", G_CALLBACK(wbutton5), NULL);
    g_signal_connect(G_OBJECT(wButton[6]), "clicked", G_CALLBACK(wbutton6), NULL);
    g_signal_connect(G_OBJECT(wButton[7]), "clicked", G_CALLBACK(wbutton7), NULL);
    g_signal_connect(G_OBJECT(wButton[8]), "clicked", G_CALLBACK(wbutton8), NULL);

    gtk_widget_show_all (window);

    gtk_main();

    return 0;
}
