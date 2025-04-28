#include "move.h"
#include <iostream>
#include <cstdio>
#include <fstream>
MoveGenerationEngine::MoveGenerationEngine(){
    this->generate_bishop_movement();
    this->generate_rook_movement();
    for(int sq = 0; sq < 64; sq++){
        this->bishop_number_of_reachable_squares[sq]
          = __builtin_popcountll(bishop_movement[sq]);
        this->rook_number_of_reachable_squares[sq]
          = __builtin_popcountll(  rook_movement[sq]);
    }
    /*this->bishop_number_of_reachable_squares = {
        7,  7,  7,  7,  7,  7,  7,  7,
        7,  9,  9,  9,  9,  9,  9,  7,
        7,  9, 11, 11, 11, 11,  9,  7,
        7,  9, 11, 13, 13, 11,  9,  7,
        7,  9, 11, 13, 13, 11,  9,  7,
        7,  9, 11, 11, 11, 11,  9,  7,
        7,  9,  9,  9,  9,  9,  9,  7,
        7,  7,  7,  7,  7,  7,  7,  7
    };
    this->rook_number_of_reachable_squares = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12
    };*/
    this->bishop_attack_masks.resize(64);
    for(int sq = 0; sq < 64; sq++){
        auto n = bishop_number_of_reachable_squares[sq];
        bishop_attack_masks[sq].assign(1ULL << n, 0ULL);
    }

    this->rook_attack_masks.resize(64);
    for(int sq = 0; sq < 64; sq++){
        auto n = rook_number_of_reachable_squares[sq];
        rook_attack_masks[sq].assign(1ULL << n, 0ULL);
    }
    this->bishop_magic_square_values = {
        0x89a1121896040240ULL,
        0x2004844802002010ULL,
        0x2068080051921000ULL,
        0x62880a0220200808ULL,
        0x4042004000000ULL,
        0x100822020200011ULL,
        0xc00444222012000aULL,
        0x28808801216001ULL,
        0x400492088408100ULL,
        0x201c401040c0084ULL,
        0x840800910a0010ULL,
        0x82080240060ULL,
        0x2000840504006000ULL,
        0x30010c4108405004ULL,
        0x1008005410080802ULL,
        0x8144042209100900ULL,
        0x208081020014400ULL,
        0x4800201208ca00ULL,
        0xf18140408012008ULL,
        0x1004002802102001ULL,
        0x841000820080811ULL,
        0x40200200a42008ULL,
        0x800054042000ULL,
        0x88010400410c9000ULL,
        0x520040470104290ULL,
        0x1004040051500081ULL,
        0x2002081833080021ULL,
        0x400c00c010142ULL,
        0x941408200c002000ULL,
        0x658810000806011ULL,
        0x188071040440a00ULL,
        0x4800404002011c00ULL,
        0x104442040404200ULL,
        0x511080202091021ULL,
        0x4022401120400ULL,
        0x80c0040400080120ULL,
        0x8040010040820802ULL,
        0x480810700020090ULL,
        0x102008e00040242ULL,
        0x809005202050100ULL,
        0x8002024220104080ULL,
        0x431008804142000ULL,
        0x19001802081400ULL,
        0x200014208040080ULL,
        0x3308082008200100ULL,
        0x41010500040c020ULL,
        0x4012020c04210308ULL,
        0x208220a202004080ULL,
        0x111040120082000ULL,
        0x6803040141280a00ULL,
        0x2101004202410000ULL,
        0x8200000041108022ULL,
        0x21082088000ULL,
        0x2410204010040ULL,
        0x40100400809000ULL,
        0x822088220820214ULL,
        0x40808090012004ULL,
        0x910224040218c9ULL,
        0x402814422015008ULL,
        0x90014004842410ULL,
        0x1000042304105ULL,
        0x10008830412a00ULL,
        0x2520081090008908ULL,
        0x40102000a0a60140ULL
    };
    this->rook_magic_square_values = {
        0xa8002c000108020ULL,
        0x6c00049b0002001ULL,
        0x100200010090040ULL,
        0x2480041000800801ULL,
        0x280028004000800ULL,
        0x900410008040022ULL,
        0x280020001001080ULL,
        0x2880002041000080ULL,
        0xa000800080400034ULL,
        0x4808020004000ULL,
        0x2290802004801000ULL,
        0x411000d00100020ULL,
        0x402800800040080ULL,
        0xb000401004208ULL,
        0x2409000100040200ULL,
        0x1002100004082ULL,
        0x22878001e24000ULL,
        0x1090810021004010ULL,
        0x801030040200012ULL,
        0x500808008001000ULL,
        0xa08018014000880ULL,
        0x8000808004000200ULL,
        0x201008080010200ULL,
        0x801020000441091ULL,
        0x800080204005ULL,
        0x1040200040100048ULL,
        0x120200402082ULL,
        0xd14880480100080ULL,
        0x12040280080080ULL,
        0x100040080020080ULL,
        0x9020010080800200ULL,
        0x813241200148449ULL,
        0x491604001800080ULL,
        0x100401000402001ULL,
        0x4820010021001040ULL,
        0x400402202000812ULL,
        0x209009005000802ULL,
        0x810800601800400ULL,
        0x4301083214000150ULL,
        0x204026458e001401ULL,
        0x40204000808000ULL,
        0x8001008040010020ULL,
        0x8410820820420010ULL,
        0x1003001000090020ULL,
        0x804040008008080ULL,
        0x12000810020004ULL,
        0x1000100200040208ULL,
        0x430000a044020001ULL,
        0x280009023410300ULL,
        0xe0100040002240ULL,
        0x200100401700ULL,
        0x2244100408008080ULL,
        0x8000400801980ULL,
        0x2000810040200ULL,
        0x8010100228810400ULL,
        0x2000009044210200ULL,
        0x4080008040102101ULL,
        0x40002080411d01ULL,
        0x2005524060000901ULL,
        0x502001008400422ULL,
        0x489a000810200402ULL,
        0x1004400080a13ULL,
        0x4000011008020084ULL,
        0x26002114058042ULL
    };

    /*if (!load_rook_masks_from_file("rook_masks.bin")) {
        save_rook_masks_to_file("rook_masks.bin");
    }
    if (!load_bishop_masks_from_file("bishop_masks.bin")) {
        save_bishop_masks_to_file("bishop_masks.bin");
    }*/
    this->generate_knight_movement();
    this->generate_king_movement();
    this->generate_pawn_movement();
    this->generate_obstacle_based_rook_movement();
    this->generate_obstacle_based_bishop_movement();
}
MoveGenerationEngine::~MoveGenerationEngine(){};
BitBoard MoveGenerationEngine::get_knight_attacks(int square) {
    return knight_movement[square];
}
BitBoard MoveGenerationEngine::get_king_attacks(int square) {
    return king_movement[square];
}
BitBoard MoveGenerationEngine::get_pawn_attacks(int square, bool is_white) {
    return is_white ? (white_pawn_left_movement[square] | white_pawn_right_movement[square])
                   : (black_pawn_left_movement[square] | black_pawn_right_movement[square]);
}
BitBoard MoveGenerationEngine::get_bishop_attacks(int square, BitBoard occupied) {
    /*BitBoard obstacles = occupied & bishop_movement[square];
    auto index = (obstacles * bishop_magic_square_values[square])
                 >> (64 - bishop_number_of_reachable_squares[square]);
    index &= (1ULL << bishop_number_of_reachable_squares[square]) - 1;
    return bishop_attack_masks[square][index];*/
    BitBoard attacks = 0ULL;
    int file = square % 8;
    int rank = square / 8;

    // up-left
    for(int r = rank-1, f = file-1; r >= 0 && f >= 0; --r, --f) {
        int to = r*8 + f;
        attacks |= 1ULL << to;
        if (occupied & (1ULL << to)) break;
    }
    // up-right
    for(int r = rank-1, f = file+1; r >= 0 && f < 8; --r, ++f) {
        int to = r*8 + f;
        attacks |= 1ULL << to;
        if (occupied & (1ULL << to)) break;
    }
    // down-right
    for(int r = rank+1, f = file+1; r < 8 && f < 8; ++r, ++f) {
        int to = r*8 + f;
        attacks |= 1ULL << to;
        if (occupied & (1ULL << to)) break;
    }
    // down-left
    for(int r = rank+1, f = file-1; r < 8 && f >= 0; ++r, --f) {
        int to = r*8 + f;
        attacks |= 1ULL << to;
        if (occupied & (1ULL << to)) break;
    }

    return attacks;
}
BitBoard MoveGenerationEngine::get_rook_attacks(int square, BitBoard occupied) {
    /*BitBoard obstacles = occupied & rook_movement[square];
    BitBoard magic_index = (obstacles * rook_magic_square_values[square]) >> 
                          (64 - rook_number_of_reachable_squares[square]);
                          magic_index &= (1ULL << rook_number_of_reachable_squares[square]) - 1;
    return rook_attack_masks[square][magic_index];*/
    BitBoard attacks = 0ULL;
    int file = square % 8;
    int rank = square / 8;

    // up
    for(int r = rank-1; r >= 0; --r) {
        int to = r*8 + file;
        attacks |= 1ULL << to;
        if (occupied & (1ULL << to)) break;
    }
    // down
    for(int r = rank+1; r < 8; ++r) {
        int to = r*8 + file;
        attacks |= 1ULL << to;
        if (occupied & (1ULL << to)) break;
    }
    // left
    for(int f = file-1; f >= 0; --f) {
        int to = rank*8 + f;
        attacks |= 1ULL << to;
        if (occupied & (1ULL << to)) break;
    }
    // right
    for(int f = file+1; f < 8; ++f) {
        int to = rank*8 + f;
        attacks |= 1ULL << to;
        if (occupied & (1ULL << to)) break;
    }

    return attacks;
}
BitBoard MoveGenerationEngine::get_queen_attacks(int square, BitBoard occupied){
    return get_bishop_attacks(square,occupied) | get_rook_attacks(square,occupied);
}

void MoveGenerationEngine::generate_knight_movement(){
    for(int sq=0;sq<64;sq++){
        BitBoard temp_bb = 0LL;
        int col = sq%8;
        if(col!=0 && sq>=17)    temp_bb |= 1LL << (sq-17);
        if(col!=7 && sq>=16)    temp_bb |= 1LL << (sq-15);
        if(col!=0 && sq<=47)    temp_bb |= 1LL << (sq+15);
        if(col!=7 && sq<=46)    temp_bb |= 1LL << (sq+17);
        if(col>1 && sq>=10)     temp_bb |= 1LL << (sq-10);
        if(col>1 && sq<=55)     temp_bb |= 1LL << (sq+6);
        if(col<6 && sq>=8)      temp_bb |= 1LL << (sq-6);
        if(col<6 && sq<=53)     temp_bb |= 1LL << (sq+10);
        this->knight_movement[sq] = temp_bb;
    
    }
}
void MoveGenerationEngine::generate_king_movement(){
    for(int sq=0;sq<64;sq++){
        BitBoard temp_bb = 0LL;
        int col = sq%8;
        if(sq>=8)   temp_bb |= 1LL << (sq-8); //up
        if(sq<=55)  temp_bb |= 1LL << (sq+8); //down
        if(col>0)   temp_bb |= 1LL << (sq-1); //left
        if(col<7)   temp_bb |= 1LL << (sq+1); //right
        if(col>0 && sq>=9)  temp_bb |= 1LL << (sq-9); //up-left diagonal
        if(col<7 && sq>=8) temp_bb |= 1LL << (sq-7); // up-right diagonal
        if(col>0 && sq<=55) temp_bb |= 1LL << (sq+7); // down-left diagonal
        if(col<7 && sq<=54) temp_bb |= 1LL << (sq+9); // down-right diagonal
        this->king_movement[sq] = temp_bb;
    }
}
void MoveGenerationEngine::generate_pawn_movement(){
    for(int sq=0;sq<64;sq++){
        BitBoard temp_bb = 0LL;
        int col = sq%8;
        if(col!=0 && sq>=9) temp_bb |= 1LL << (sq-9);
        this->white_pawn_left_movement[sq] = temp_bb;
        temp_bb = 0LL;
        if(col!=7 && sq>=8) temp_bb |= 1LL << (sq-7);
        this->white_pawn_right_movement[sq] = temp_bb;
        temp_bb = 0LL;
        if(col!=7 && sq<=54)    temp_bb |= 1LL << (sq+9);
        this->black_pawn_left_movement[sq] = temp_bb;
        temp_bb = 0LL;
        if(col!=0 && sq<=55)    temp_bb |= 1LL << (sq+7);
        this->black_pawn_right_movement[sq] = temp_bb;
    }
}
void MoveGenerationEngine::generate_bishop_movement() {
    for (int sq = 0; sq < 64; sq++) {
        BitBoard temp_bb = 0ULL;
        int col = sq % 8;
        int row = sq / 8;
        {
            int r = row - 1;
            int c = col - 1;
            while (r >= 0 && c >= 0) {
                int target = r * 8 + c;
                temp_bb |= 1ULL << target;
                r--;
                c--;
            }
        }
        {
            int r = row - 1;
            int c = col + 1;
            while (r >= 0 && c < 8) {
                int target = r * 8 + c;
                temp_bb |= 1ULL << target;
                r--;
                c++;
            }
        }
        {
            int r = row + 1;
            int c = col + 1;
            while (r < 8 && c < 8) {
                int target = r * 8 + c;
                temp_bb |= 1ULL << target;
                r++;
                c++;
            }
        }
        {
            int r = row + 1;
            int c = col - 1;
            while (r < 8 && c >= 0) {
                int target = r * 8 + c;
                temp_bb |= 1ULL << target;
                r++;
                c--;
            }
        }

        this->bishop_movement[sq] = temp_bb;
    }
}
void MoveGenerationEngine::generate_rook_movement(){
    for(int sq=0; sq<64; sq++){
        BitBoard temp_bb = 0LL;
        int temp_sq = sq-8;
        while(temp_sq>=0){
            temp_bb |= 1LL << temp_sq;
            temp_sq -= 8;
        }
        temp_sq = sq+8;
        while(temp_sq<64){
            temp_bb |= 1LL << temp_sq;
            temp_sq += 8;
        }
        temp_sq = sq - 1;
        while (temp_sq >= 0 && temp_sq / 8 == sq / 8){
            temp_bb |= 1LL << temp_sq;
            temp_sq--;
        }
        temp_sq = sq + 1;
        while (temp_sq < 64 && temp_sq / 8 == sq / 8){
            temp_bb |= 1LL << temp_sq;
            temp_sq++;
        }
        this->rook_movement[sq] = temp_bb;
    }
}
void MoveGenerationEngine::generate_obstacle_based_bishop_movement(){
    this->generate_bishop_movement();
    for (int sq = 0; sq < 64; sq++){
        BitBoard bishop_future_moves = this->bishop_movement[sq];
        int number_of_reachable_squares = bishop_number_of_reachable_squares[sq];
        int number_of_reachable_square_combinations = 1LL << number_of_reachable_squares;
        for (int i = 0; i < number_of_reachable_square_combinations; i++){
            BitBoard obstacle_map = generate_obstacle_map(i, number_of_reachable_squares, bishop_future_moves);
            BitBoard magic_index = (obstacle_map * this->bishop_magic_square_values[sq]) >>
                                   (64 - number_of_reachable_squares);
            magic_index &= (1ULL << number_of_reachable_squares) - 1;
            this->bishop_attack_masks[sq][magic_index] = generate_bishop_attacks_with_obstacles(sq, obstacle_map);
        }
    }
}

void MoveGenerationEngine::generate_obstacle_based_rook_movement(){
    this->generate_rook_movement();
    for(int sq=0;sq<64;sq++){
        BitBoard future_rook_moves = this->rook_movement[sq];
        int number_of_reachable_squares = rook_number_of_reachable_squares[sq];
        int number_of_reachable_square_combinations = 1LL << number_of_reachable_squares;
        for(int i=0;i<number_of_reachable_square_combinations;i++){
            BitBoard obstacle_map = generate_obstacle_map(i,number_of_reachable_squares,future_rook_moves);
            BitBoard magic_index = (obstacle_map * this->rook_magic_square_values[sq]) >> (64-number_of_reachable_squares);
            magic_index &= (1ULL << number_of_reachable_squares) - 1;
            this->rook_attack_masks[sq][magic_index] = this->generate_rook_attacks_with_obstacles(sq,obstacle_map);
        }
    }
}
BitBoard MoveGenerationEngine::generate_obstacle_map(int current_obstacle_combination, int number_of_bits, BitBoard future_moves){
    BitBoard obstacle_map = 0LL;
    for(int temp_bit = 0; temp_bit < number_of_bits;temp_bit++){
        int occupied_square = __builtin_ctzll(future_moves);
        future_moves = future_moves ^ (1LL << occupied_square);
        if (current_obstacle_combination & (1<<temp_bit)){
            obstacle_map |= (1LL<<occupied_square);
        }
    }
    return obstacle_map;
}
BitBoard MoveGenerationEngine::generate_bishop_attacks_with_obstacles(int sq, BitBoard obstacle_map){
    BitBoard temp_bb = 0LL;
    int temp_up_left_diagonal_sq = sq;
    int temp_up_right_diagonal_sq = sq;
    int temp_down_left_diagonal_sq = sq;
    int temp_down_right_diagonal_sq = sq;
    int temp_col = sq%8;
    int temp_row = sq/8;
    while(true){
        temp_up_left_diagonal_sq -= 9;
        if(temp_up_left_diagonal_sq<0){
            break;
        }
        if(temp_up_left_diagonal_sq%8!=temp_col-1 || temp_up_left_diagonal_sq/8!=temp_row-1){
            break;
        }
        temp_col = temp_up_left_diagonal_sq%8;
        temp_row = temp_up_left_diagonal_sq/8;
        temp_bb |= 1LL << (temp_up_left_diagonal_sq);
        if(obstacle_map & (1LL << (temp_up_left_diagonal_sq))){
            break;
        }
    }
    temp_col = sq%8;
    temp_row = sq/8;
    while(true){
        temp_up_right_diagonal_sq -= 7;
        if(temp_up_right_diagonal_sq<0){
            break;
        }
        if(temp_up_right_diagonal_sq%8!=temp_col+1 || temp_up_right_diagonal_sq/8!=temp_row-1){
            break;
        }
        temp_col = temp_up_right_diagonal_sq%8;
        temp_row = temp_up_right_diagonal_sq/8;
        temp_bb |= 1LL << (temp_up_right_diagonal_sq);
        if(obstacle_map & (1LL << (temp_up_right_diagonal_sq))){
            break;
        }
    }
    temp_col = sq%8;
    temp_row = sq/8;
    while(true){
        temp_down_right_diagonal_sq += 9;
        if(temp_down_right_diagonal_sq>63){
            break;
        }
        if(temp_down_right_diagonal_sq%8!=temp_col+1 || temp_down_right_diagonal_sq/8!=temp_row+1){
            break;
        }
        temp_col = temp_down_right_diagonal_sq%8;
        temp_row = temp_down_right_diagonal_sq/8;
        temp_bb |= 1LL << (temp_down_right_diagonal_sq);
        if(obstacle_map & (1LL << (temp_down_right_diagonal_sq))){
            break;
        }
    }
    temp_col = sq%8;
    temp_row = sq/8;
    while(true){
        temp_down_left_diagonal_sq += 7;
        if(temp_down_left_diagonal_sq>63){
            break;
        }
        if(temp_down_left_diagonal_sq%8!=temp_col-1 || temp_down_left_diagonal_sq/8!=temp_row+1){
            break;
        }
        temp_col = temp_down_left_diagonal_sq%8;
        temp_row = temp_down_left_diagonal_sq/8;
        temp_bb |= 1LL << (temp_down_left_diagonal_sq);
        if(obstacle_map & (1LL << (temp_down_left_diagonal_sq))){
            break;
        }
    }
    return temp_bb;
}
BitBoard MoveGenerationEngine::generate_rook_attacks_with_obstacles(int sq, BitBoard obstacle_map) {
    BitBoard temp_bb = 0LL;

    // Up
    int temp_sq = sq - 8;
    while (temp_sq >= 0) {
        temp_bb |= 1LL << temp_sq;
        if (obstacle_map & (1LL << temp_sq)) break;
        temp_sq -= 8;
    }

    // Down
    temp_sq = sq + 8;
    while (temp_sq < 64) {
        temp_bb |= 1LL << temp_sq;
        if (obstacle_map & (1LL << temp_sq)) break;
        temp_sq += 8;
    }

    // Left
    temp_sq = sq - 1;
    while (temp_sq >= 0 && temp_sq / 8 == sq / 8) {
        temp_bb |= 1LL << temp_sq;
        if (obstacle_map & (1LL << temp_sq)) break;
        temp_sq--;
    }

    // Right
    temp_sq = sq + 1;
    while (temp_sq < 64 && temp_sq / 8 == sq / 8) {
        temp_bb |= 1LL << temp_sq;
        if (obstacle_map & (1LL << temp_sq)) break;
        temp_sq++;
    }

    return temp_bb;
}
BitBoard MoveGenerationEngine::get_pawn_white_right(int sq){
    return white_pawn_right_movement[sq];
}
BitBoard MoveGenerationEngine::get_pawn_white_left(int sq){
    return white_pawn_left_movement[sq];
}
void MoveGenerationEngine::save_rook_masks_to_file(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    for (int sq = 0; sq < 64; sq++) {
        out.write(reinterpret_cast<const char*>(rook_attack_masks[sq].data()), 4096 * sizeof(BitBoard));
    }
    out.close();
}
bool MoveGenerationEngine::load_rook_masks_from_file(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;
    rook_attack_masks.resize(64, std::vector<BitBoard>(4096));
    for (int sq = 0; sq < 64; sq++) {
        in.read(reinterpret_cast<char*>(rook_attack_masks[sq].data()), 4096 * sizeof(BitBoard));
    }
    in.close();
    return true;
}
void MoveGenerationEngine::save_bishop_masks_to_file(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    for (int sq = 0; sq < 64; sq++) {
        out.write(reinterpret_cast<const char*>(bishop_attack_masks[sq].data()), 512 * sizeof(BitBoard));
    }
    out.close();
}
bool MoveGenerationEngine::load_bishop_masks_from_file(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;
    bishop_attack_masks.resize(64, std::vector<BitBoard>(512));
    for (int sq = 0; sq < 64; sq++) {
        in.read(reinterpret_cast<char*>(bishop_attack_masks[sq].data()), 512 * sizeof(BitBoard));
    }
    in.close();
    return true;
}


