/***************************************************************************
 *            TrisMachine.h
 *
 *  Mon Apr 16 18:37:59 2007
 *  Copyright  2007  Simone Tobia
 *  Email simone_tobia@tiscali.it
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 // v0.7

#ifndef TRISMACHINE
#define TRISMACHINE
#include "Tavola.h"

enum ia_level{STUPIDA, NORMALE, SVEGLIA, HARDCORE};

class TrisMachine{
    protected:
        static bool is_verbose_active;
        inline static int find_first_free(const Tavola &, int &);
        inline static int check_central(const Tavola &p, int &row){return (p.libera(p.get_dimensione()/2, p.get_dimensione()/2)&&p.get_occupate()<2?(row=p.get_dimensione()/2):-1);};
        inline static int check_all_attacks(const Tavola &, int &, const bool, ia_level);
        inline static int check_attacks(const Tavola &, int &, const bool, const int &, ia_level);
        inline static int check_row_attacks(const Tavola &, int &, const bool, const int &, ia_level);
        inline static int check_col_attacks(const Tavola &, int &, const bool, const int &, ia_level);
        inline static int check_diag_attacks(const Tavola &, int &, const bool, const int &, ia_level);
        inline static int check_all_diag_attacks(const Tavola &, int &, const bool, const int &, ia_level);
        inline static int checkAngles(const Tavola &p, int &row,const bool croce);
        inline static int count_sym_row(const Tavola &, const bool, const unsigned int &);//conta i simboli su una riga.
        inline static int count_sym_col(const Tavola &, const bool, const unsigned int &);//conta i simboli su una colonna.
        inline static int count_sym_diag(const Tavola &, const bool, const unsigned int &);//Conta i siboli su una diagonale.
        inline static int count_sym_all_diag(const Tavola &, const bool, const unsigned int &, const bool first);//Conta i siboli su una diagonale(esteso alle diagonali non principali).

        inline static int find_row_defense(const Tavola &, int ,const bool, const int &);
        inline static int find_col_defense(const Tavola &, int ,const bool, const int &);
        inline static int find_diag_defense(const Tavola &p, bool first, const bool croce, const int & proximity);
        inline static int find_all_diag_defense(const Tavola &p, bool first, const bool croce, const int & proximity, const unsigned int &diag);

        inline static bool check_col_possibility(const Tavola &p, int col, const bool croce, const bool iscol);
        inline static bool check_diag_possibility(const Tavola &p, bool first, const bool croce);
        inline static bool check_all_diag_possibility(const Tavola &p, bool first, const bool croce, const unsigned int &);

        inline static int find_cross_solutions(const Tavola &p, const bool &croce, int &riga, const int &proximity);
        inline static int count_sequence(const Tavola &p, bool croce, unsigned int row, unsigned int col, bool iscol);
    public:
        static int get_move(Tavola &, int &, const bool, ia_level);//Ritorna il numero di colonna della mossa e il numero di riga nel terzo parametro passato come riferimento.
        static void on_verbose(){is_verbose_active=true;};
        static void off_verbose(){is_verbose_active=false;};
};

#endif
