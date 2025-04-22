#include <iostream>
#include <ctime>
#include <cstdio>
#include <iomanip>
using namespace std;

#define SYMBOL_SCIANY 254;
#define SYMBOL_POLA 176;

struct Plansza {
    int rozmiar_planszy;
    int liczba_ukrytych_atomow;
    char** mapa;
    char** mapa_atomow;
};

struct Ruch {
    int x;
    int y;
    char skutek_ruchu;
};

struct Stos {
    int liczba_ruchow = 0;
    int indeks_ostatniego_ruchu = 0;
    Ruch* ruchy_gracza;
};

void wyswietl_ekran_powitalny() {
    cout <<
        "_______   ___      _______  _______  ___   _    _______  _______  __   __\n"
        "|  _    ||   |    |   _   ||       ||   | | |  |  _    ||       ||  |_|  |\n"
        "| |_|   ||   |    |  |_|  ||       ||   |_| |  | |_|   ||   _   ||       |\n"
        "|       ||   |    |       ||       ||      _|  |       ||  | |  ||       |\n"
        "|  _   | |   |___ |       ||      _||     |_   |  _   | |  |_|  | |     |\n"
        "| |_|   ||       ||   _   ||     |_ |    _  |  | |_|   ||       ||   _   |\n"
        "|_______||_______||__| |__||_______||___| |_|  |_______||_______||__| |__|" << endl;
    cout << "Autor: Zuzanna Borkowska 193678 IBM gr 1" << endl;
    cout << "Witaj w grze Black Box!\n"
        "Sterowanie:\n"
        "w,s,a,d (W, S, A, D): poruszanie sie po planszy odpowiednio: w gore, w dol, w lewo i w prawo\n"
        "q, Q: przerwanie gry\n"
        "u, U: cofnij ruch\n"
        "r, R: powtorz cofniety ruch\n"
        "spacja: oddanie strzalu\n"
        "o: zaznaczenie przypuszczalnego polozenia atomu\n"
        "k: koniec rozgrywki - wyswietlenie liczby poprawnie zaznaczonych atomow i rozwiazania\n"
        "p: poddanie sie -  wyswietlenie rozwiazania\n"
        "H: pokazuje rozmieszczenie atomow na planszy" << endl;
}

void menu_wybor_poziomu() {
	cout << "Wybierz poziom trudnosci:\n"
		"1. Latwy (5x5)\n"
		"2. Sredni (8x8)\n"
		"3. Trudny (10x10)\n";
}

void wybierz_poziom_gry(Plansza& p) {
    cin >> p.rozmiar_planszy;
    switch (p.rozmiar_planszy) {
    case 5:
        p.liczba_ukrytych_atomow = 3;
        break;
    case 8:
        p.liczba_ukrytych_atomow = 5;
        break;
    case 10:
        p.liczba_ukrytych_atomow = 8;
        break;
    }
}

void generuj_plansze(Plansza& p) {
    int rozmiar = p.rozmiar_planszy + 2;

    p.mapa = new char* [rozmiar];
    p.mapa_atomow = new char* [rozmiar];
    for (int i = 0; i < rozmiar; i++) {
        p.mapa[i] = new char[rozmiar];
        p.mapa_atomow[i] = new char[rozmiar];
    }

    for (int y = 0; y < rozmiar; y++) {
        for (int x = 0; x < rozmiar; x++) {
            if (x == 0 || x == rozmiar - 1 || y == 0 || y == rozmiar - 1) {
                p.mapa[y][x] = SYMBOL_SCIANY;
                p.mapa_atomow[y][x] = SYMBOL_SCIANY;
            }
            else {
                p.mapa[y][x] = SYMBOL_POLA;
                p.mapa_atomow[y][x] = SYMBOL_POLA;
            }
        }
    }
}

void losuj_polozenie_atomow(Plansza& p) {

    int n = p.liczba_ukrytych_atomow;
    int rozmiar = p.rozmiar_planszy;
    for (int i = 0; i < n; i++) {
        int x = (rand() % rozmiar) + 1;
        int y = (rand() % rozmiar) + 1;

        if (p.mapa_atomow[y][x] == '*') {
            i--;
        }
        else {
            p.mapa_atomow[y][x] = '*';
        }
    }
}

void dodaj_ruch(Stos& stos, int x, int y, char skutek_ruchu) {
    Ruch ruch;
    ruch.x = x;
    ruch.y = y;
    ruch.skutek_ruchu = skutek_ruchu;
    stos.ruchy_gracza[stos.liczba_ruchow] = ruch;
    stos.liczba_ruchow++;
    stos.indeks_ostatniego_ruchu = stos.liczba_ruchow;
}

void cofnij_ruch(Plansza& p, Stos& stos) {
    if (stos.liczba_ruchow - 1 < 0) {
        return;
    }
    Ruch ostatni_ruch = stos.ruchy_gracza[stos.liczba_ruchow - 1];
    if (ostatni_ruch.skutek_ruchu == 'o') {
        p.mapa[ostatni_ruch.y][ostatni_ruch.x] = SYMBOL_POLA;
    }
    else {
        p.mapa[ostatni_ruch.y][ostatni_ruch.x] = SYMBOL_SCIANY;
    }
    stos.liczba_ruchow--;
}

void powtorz_cofniety_ruch(Plansza& p, Stos& stos) {
    if (stos.liczba_ruchow >= stos.indeks_ostatniego_ruchu)
    {
        return;
    }
    stos.liczba_ruchow++;
    Ruch ostatni_ruch = stos.ruchy_gracza[stos.liczba_ruchow - 1];
    p.mapa[ostatni_ruch.y][ostatni_ruch.x] = ostatni_ruch.skutek_ruchu;
}

void wypisz_plansze(Plansza& p, bool czy_pokazac_atomy, int x_gracza, int y_gracza) {
    cout << "W tym etapie ukrytych jest " << p.liczba_ukrytych_atomow << " atomow." << endl;
    for (int y = 0; y < p.rozmiar_planszy + 2; y++) {
        for (int x = 0; x < p.rozmiar_planszy + 2; x++) {
            if (x == x_gracza && y == y_gracza) {
                cout << setw(2) << 'G';
            }
            else if (czy_pokazac_atomy) {
                cout << setw(2) << p.mapa_atomow[y][x];
            }
            else {
                cout << setw(2) << p.mapa[y][x];
            }
        }
        cout << endl;
    }
}

bool strzal(Plansza& p, int x_strzalu, int y_strzalu, int x_max, int y_max, Stos& stos) {
    if ((x_strzalu == 0 || x_strzalu == x_max) && (y_strzalu == 0 || y_strzalu == y_max)) {
        return false;
    }
    if (x_strzalu == 0) {
        bool atom_trafiony = false;
        bool atom_pod_nad = false;

        for (int x = 1; x <= p.rozmiar_planszy; x++) {
            if (p.mapa_atomow[y_strzalu][x] == '*') {
                atom_trafiony = true;
                break;
            }
            else if ((y_strzalu < p.rozmiar_planszy && p.mapa_atomow[y_strzalu + 1][x] == '*') || (y_strzalu > 1 && p.mapa_atomow[y_strzalu - 1][x] == '*')) {
                atom_pod_nad = true;

            }
        }
        p.mapa[y_strzalu][x_max] = atom_trafiony ? 'H' : (atom_pod_nad ? 'R' : 'M');
        dodaj_ruch(stos, x_max, y_strzalu, p.mapa[y_strzalu][x_max]);
    }
    if (x_strzalu == x_max) {
        bool atom_trafiony = false;
        bool atom_pod_nad = false;

        for (int x = p.rozmiar_planszy; x >= 1; x--) {
            if (p.mapa_atomow[y_strzalu][x] == '*') {
                atom_trafiony = true;
                break;
            }
            else if ((y_strzalu > 1 && p.mapa_atomow[y_strzalu - 1][x] == '*') || (y_strzalu < p.rozmiar_planszy && p.mapa_atomow[y_strzalu + 1][x] == '*')) {
                atom_pod_nad = true;
            }

        }
        p.mapa[y_strzalu][0] = atom_trafiony ? 'H' : (atom_pod_nad ? 'R' : 'M');
        dodaj_ruch(stos, 0, y_strzalu, p.mapa[y_strzalu][0]);
    }
    if (y_strzalu == 0) {
        bool atom_trafiony = false;
        bool atom_obok = false;

        for (int y = 1; y <= p.rozmiar_planszy; y++) {
            if (p.mapa_atomow[y][x_strzalu] == '*') {
                atom_trafiony = true;
                break;
            }
            else if ((x_strzalu > 1 && p.mapa_atomow[y][x_strzalu - 1] == '*') ||
                (x_strzalu < p.rozmiar_planszy && p.mapa_atomow[y][x_strzalu + 1] == '*')) {
                atom_obok = true;
            }
        }
        p.mapa[y_max][x_strzalu] = atom_trafiony ? 'H' : (atom_obok ? 'R' : 'M');
        dodaj_ruch(stos, x_strzalu, y_max, p.mapa[y_max][x_strzalu]);
    }
    if (y_strzalu == y_max) {
        bool atom_trafiony = false;
        bool atom_obok = false;

        for (int y = p.rozmiar_planszy; y >= 1; y--) {
            if (p.mapa_atomow[y][x_strzalu] == '*') {
                atom_trafiony = true;
                break;
            }
            else if ((x_strzalu > 1 && p.mapa_atomow[y][x_strzalu - 1] == '*') ||
                (x_strzalu < p.rozmiar_planszy && p.mapa_atomow[y][x_strzalu + 1] == '*')) {
                atom_obok = true;
            }
        }
        p.mapa[0][x_strzalu] = atom_trafiony ? 'H' : (atom_obok ? 'R' : 'M');
        dodaj_ruch(stos, x_strzalu, 0, p.mapa[0][x_strzalu]);
    }
}

void sterowanie(int& x_gracza, int& y_gracza, int x_max, int y_max, bool& czy_pokazac_atomy, bool& czy_przerwac_gre, bool& koniec_gry, bool& czy_poddajesz_sie, Plansza& p, Stos& stos, int& skutek_ruchu_gry) {
    char operacja;
    operacja = getchar();

    switch (operacja) {
    case 'w':
    case 'W':
        if (y_gracza > 0) {
            y_gracza--;
        }
        break;
    case 's':
    case 'S':
        if (y_gracza < y_max) {
            y_gracza++;
        }
        break;
    case 'a':
    case 'A':
        if (x_gracza > 0) {
            x_gracza--;
        }
        break;
    case 'd':
    case 'D':
        if (x_gracza < x_max) {
            x_gracza++;
        }
        break;
    case 'H':
        czy_pokazac_atomy = !czy_pokazac_atomy;
        break;
    case 'q':
    case 'Q':
        czy_przerwac_gre = true;
        break;
    case ' ':
        strzal(p, x_gracza, y_gracza, x_max, y_max, stos);
        break;
    case 'p':
        czy_poddajesz_sie = true;
        break;
    case 'o':
        if (x_gracza > 0 && x_gracza <= p.rozmiar_planszy && y_gracza > 0 && y_gracza <= p.rozmiar_planszy) {
            int liczba_zaznaczonych_atomow = 0;
            for (int y = 1; y <= p.rozmiar_planszy; y++) {
                for (int x = 1; x <= p.rozmiar_planszy; x++) {
                    if (p.mapa[y][x] == 'o') {
                        liczba_zaznaczonych_atomow++;
                    }
                }
            }
            if (liczba_zaznaczonych_atomow < p.liczba_ukrytych_atomow) {
                p.mapa[y_gracza][x_gracza] = 'o';
                dodaj_ruch(stos, x_gracza, y_gracza, 'o');
            }
        }
        break;
    case 'U':
    case 'u':
        cofnij_ruch(p, stos);
        break;
    case 'R':
    case 'r':
        powtorz_cofniety_ruch(p, stos);
        break;
    case 'k':
        for (int y = 1; y <= p.rozmiar_planszy; y++) {
            for (int x = 1; x <= p.rozmiar_planszy; x++) {
                if (p.mapa[y][x] == 'o' && p.mapa_atomow[y][x] == '*') {
                    p.mapa[y][x] = 'O';
                    skutek_ruchu_gry++;
                }
                else if (p.mapa_atomow[y][x] == '*') {
                    p.mapa[y][x] = '*';
                }
                else if (p.mapa[y][x] == 'o' && p.mapa_atomow[y][x] != '*') {
                    p.mapa[y][x] = 'X';
                }
            }
        }
        koniec_gry = true;
        break;
    }
}

void wyczysc_ekran() {
    system("cls");
}

int main()
{
    Plansza wybrana_plansza;
    int xGracz = 0;
    int yGracz = 0;
    bool pomoc = false;
    bool przerwanie_gry = false;
    bool poddanie_sie = false;
    bool koniec_gry = false;
    int skutek_ruchu_gry = 0;
    Stos stos;
    int maksymalna_liczba_ruchow_do_cofniecia = 100;
    stos.ruchy_gracza = new Ruch[maksymalna_liczba_ruchow_do_cofniecia];

    srand(time(NULL));

    wyswietl_ekran_powitalny();

    while (true) {
        menu_wybor_poziomu();
        wybierz_poziom_gry(wybrana_plansza);
        generuj_plansze(wybrana_plansza);
        losuj_polozenie_atomow(wybrana_plansza);

        while (true) {
            wypisz_plansze(wybrana_plansza, pomoc, xGracz, yGracz);
            sterowanie(xGracz, yGracz, wybrana_plansza.rozmiar_planszy + 1, wybrana_plansza.rozmiar_planszy + 1, pomoc, przerwanie_gry, koniec_gry, poddanie_sie, wybrana_plansza, stos, skutek_ruchu_gry);
            wyczysc_ekran();
            if (przerwanie_gry) {
                cout << "Gra przerwana." << endl;
                break;
            }
            if (poddanie_sie) {
                wypisz_plansze(wybrana_plansza, true, xGracz, yGracz);
                break;
            }
            if (koniec_gry) {
                wypisz_plansze(wybrana_plansza, false, xGracz, yGracz);
                cout << "Liczba poprawnie zaznaczonych atomow: " << skutek_ruchu_gry << endl;
                break;
            }
        }
        int rozmiar = wybrana_plansza.rozmiar_planszy + 2;
        for (int i = 0; i < rozmiar; i++) {
            delete[] wybrana_plansza.mapa[i];
            delete[] wybrana_plansza.mapa_atomow[i];
        }
        delete[] wybrana_plansza.mapa;
        delete[] wybrana_plansza.mapa_atomow;
        delete[] stos.ruchy_gracza;

        char wybor;
        cout << "Czy chcesz zagrac jeszcze raz?[T/N]" << endl;
        cin >> wybor;
        if (wybor != 'T' && wybor != 't') {
            cout << "Do zobaczenia!";
            break;
        }
    }
    return 0;
}