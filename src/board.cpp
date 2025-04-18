#include "board.h"
#include <stdlib.h>  
#include <ctype.h>
#include <cstring>
#include "move.h"
#include "evaluation.h"
Board::Board(MoveGenerationEngine& moveEngine, EvaluationEngine& evaluationEngine):moveEngine(moveEngine),evaluationEngine(evaluationEngine){
    for(int i=0;i<64;i++){
        BitBoard tempBB = 1ULL << i;
        this->board_squares[i] = tempBB;
    }
    this->init_zobrist_hashing();
    this->init_piece_value_map();
}

Board::~Board(){}

BitBoard Board::generate_random_bitboard(){
    BitBoard temp_bb = 0;
    for(int i=0;i<64;i++){
        temp_bb |= ((BitBoard) rand() % 2) << i;
    }
    return temp_bb;
}

void Board::init_piece_value_map(){
    this->piece_value_map[PAWN_B] = 0;
    this->piece_value_map[PAWN_W] = 1;
    this->piece_value_map[KNIGHT_B] = 2;
    this->piece_value_map[KNIGHT_W] = 3;
    this->piece_value_map[ROOK_B] = 4;
    this->piece_value_map[ROOK_W] = 5;
    this->piece_value_map[BISHOP_B] = 6;
    this->piece_value_map[BISHOP_W] = 7;
    this->piece_value_map[QUEEN_B] = 8;
    this->piece_value_map[QUEEN_W] = 9;
    this->piece_value_map[KING_B] = 10;
    this->piece_value_map[KING_W] = 11;
}
void Board::init_zobrist_hashing(){
    for(int i=0;i<12;i++){
        for(int j=0;j<64;j++){
            this->zobrist_pieces_hash[i][j] = generate_random_bitboard();
        }
    }
    for(int i=0;i<64;i++){
        this->zobrist_en_passant[i] = generate_random_bitboard();
    }
    for(int i=0;i<16;i++){
        this->zobrist_castling[i] = generate_random_bitboard();
    }
    this->zobrist_white_to_move = generate_random_bitboard();
}
void Board::parsePosition(char* command){
    command += 9;
    if (strstr(command, "fen")) {
        char* fen = strstr(command, "fen") + 4;
        this->evaluateFen(fen);
    }
    else if(strstr(command, "startpos")){
        this->evaluateFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
    if(strstr(command,"moves")){
        char* moves = strstr(command, "moves")+6;
        while(*moves){
            this->makeMove(moves);
            while(*moves && *moves!=' ') moves++;
            moves++;
        }
    }
}
void Board::evaluateFen(char* fen){
    resetBoard();
    int row = 0;
    int col = 0;
    int board_piece;
    int fen_section = 0;
    while(*fen && fen_section < 6){
        char c = *fen;
        if (fen_section == 0) {
            if (c >= '1' && c <= '8') {
                col += (c - '0');
            } 
            else if (c == '/') {
                row++;
                col = 0;
            } 
            else {
                board_piece = charToPiece(c);
                if (board_piece != -1) {
                    int square = row * 8 + col;
                    BitBoard* temp_bb = getPieceBitBoard(board_piece);
                    *temp_bb |= 1ULL << square;
                    col++;
                }
            }
        }
        else if(fen_section==1){
            this->turn = c=='w' ? 1 : -1;
        }
        else if(fen_section==2){
            if(c=='K') this->castling |= 1;
            if(c=='Q') this->castling |= 1<<1;
            if(c=='k') this->castling |= 1<<2;
            if(c=='q') this->castling |= 1<<3;
        }
        else if(fen_section==3){
            if (c != '-') {
                int epCol = tolower(c) - 'a';
                fen++;
                int epRow = (*fen - '1'); 
                this->enPassantSq = epRow * 8 + epCol;
            }
        }
        else if (fen_section == 4) {
            this->halfMoveCount = atoi(fen);
            while (*fen && *fen != ' ') fen++;
            continue;
        }
        else if(fen_section == 5){
            this->fullMoveCount = atoi(fen);
            break;
        }
        if (c == ' ') {
            fen_section++;
        }
        fen++;
    }
    this->whiteKingSq = __builtin_ctzll(this->white_king);
    this->blackKingSq = __builtin_ctzll(this->black_king);
    this->generateOccupancyMask();
    this->generateBoardHash();
    this->generateAttackMasks();
}
int Board::charToPiece(char c) {
    switch (c) {
        case 'P': return PAWN_W;
        case 'N': return KNIGHT_W;
        case 'B': return BISHOP_W;
        case 'R': return ROOK_W;
        case 'Q': return QUEEN_W;
        case 'K': return KING_W;
        case 'p': return PAWN_B;
        case 'n': return KNIGHT_B;
        case 'b': return BISHOP_B;
        case 'r': return ROOK_B;
        case 'q': return QUEEN_B;
        case 'k': return KING_B;
        default: return -1;
    }
}
BitBoard* Board::getPieceBitBoard(int piece){
    switch (piece) {
        case PAWN_W:   return &this->white_pawn;
        case KNIGHT_W: return &this->white_knight;
        case BISHOP_W: return &this->white_bishop;
        case ROOK_W:   return &this->white_rook;
        case QUEEN_W:  return &this->white_queen;
        case KING_W:   return &this->white_king;
        case PAWN_B:   return &this->black_pawn;
        case KNIGHT_B: return &this->black_knight;
        case BISHOP_B: return &this->black_bishop;
        case ROOK_B:   return &this->black_rook;
        case QUEEN_B:  return &this->black_queen;
        case KING_B:   return &this->black_king;
        default:       return NULL;
    }
}
void Board::resetBoard(){
    this->black_pawn = 0LL;
    this->white_pawn = 0LL;
    this->black_knight = 0LL;
    this->white_knight = 0LL;
    this->black_bishop = 0LL;
    this->white_bishop = 0LL;
    this->black_rook = 0LL;
    this->white_rook = 0LL;
    this->black_queen = 0LL;
    this->white_queen = 0LL;
    this->black_king = 0LL;
    this->white_king = 0LL;
    this->turn = 0;
    this->castling = 0;
    this->enPassantSq = -1;
}
void Board::generateOccupancyMask(){
    this->board_occupancy_white = this->white_pawn | this->white_knight | this->white_bishop | this->white_rook | this->white_queen | this->white_king; 
    this->board_occupancy_black = this->black_pawn | this->black_knight | this->black_bishop | this->black_rook | this->black_queen | this->black_king; 
    this->board_occupancy = this->board_occupancy_white | this->board_occupancy_black;
}
void Board::generateBoardHash(){
    this->board_hash = 0LL;
    this->board_hash ^= this->zobrist_castling[this->castling];
    int index = 0;
    computeHashAgainstPiece(&board_hash,this->black_pawn,index);
    computeHashAgainstPiece(&board_hash,this->white_pawn,index);
    computeHashAgainstPiece(&board_hash,this->black_knight,index);
    computeHashAgainstPiece(&board_hash,this->white_knight,index);
    computeHashAgainstPiece(&board_hash,this->black_bishop,index);
    computeHashAgainstPiece(&board_hash,this->white_bishop,index);
    computeHashAgainstPiece(&board_hash,this->black_rook,index);
    computeHashAgainstPiece(&board_hash,this->white_rook,index);
    computeHashAgainstPiece(&board_hash,this->black_queen,index);
    computeHashAgainstPiece(&board_hash,this->white_queen,index);
    computeHashAgainstPiece(&board_hash,this->black_king,index);
    computeHashAgainstPiece(&board_hash,this->white_king,index);
    if(this->enPassantSq!=-1){
        this->board_hash ^= this->zobrist_en_passant[this->enPassantSq];
    }
    if(this->turn==1){
        this->board_hash ^= this->zobrist_white_to_move;
    }
}
void Board::computeHashAgainstPiece(BitBoard* hash_pointer, BitBoard piece, int& index){
    BitBoard hash = *hash_pointer;
    while(piece){
        int square = __builtin_ctzll(piece);
            hash ^= this->zobrist_pieces_hash[index][square];
            piece &= piece -1;
    }
    index++;
}
void Board::generateAttackMasks(){
    this->board_attack_mask = 0LL;
    BitBoard pawn = this->turn==1 ? this->white_pawn : this->black_pawn;
    BitBoard knight = this->turn==1 ? this->white_knight : this->black_knight;
    BitBoard bishop = this->turn==1 ? this->white_bishop : this->black_bishop;
    BitBoard rook = this->turn==1 ? this->white_rook : this->black_rook;
    BitBoard queen = this->turn==1 ? this->white_queen : this->black_queen;
    BitBoard king = this->turn==1 ? this->white_king : this->black_king;
    while(queen){
        int sq = __builtin_ctzll(queen);
        BitBoard temp_attack = this->moveEngine.get_bishop_attacks(sq,this->board_occupancy);
        temp_attack |= this->moveEngine.get_rook_attacks(sq,this->board_occupancy);
        this->board_attack_mask |= temp_attack;
        queen &= queen-1;
    }
    while(bishop){
        int sq = __builtin_ctzll(bishop);
        BitBoard temp_attack = this->moveEngine.get_bishop_attacks(sq,this->board_occupancy);
        this->board_attack_mask |= temp_attack;
        bishop &= bishop-1;
    }
    while(rook){
        int sq = __builtin_ctzll(rook);
        BitBoard temp_attack = this->moveEngine.get_rook_attacks(sq,this->board_occupancy);
        this->board_attack_mask |= temp_attack;
        rook &= rook-1;
    }
    while(knight){
        int sq = __builtin_ctzll(knight);
        BitBoard temp_attack = this->moveEngine.get_knight_attacks(sq);
        this->board_attack_mask |= temp_attack;
        knight &= knight - 1;
    }
    while(pawn){
        int sq = __builtin_ctzll(pawn);
        BitBoard temp_attack = this->moveEngine.get_pawn_attacks(sq,this->turn==1);
        this->board_attack_mask |= temp_attack;
        pawn &= pawn - 1;
    }
    while(king){
        int sq = __builtin_ctzll(king);
        BitBoard temp_attack = this->moveEngine.get_king_attacks(sq);
        this->board_attack_mask |= temp_attack;
        king &= king-1;
    }
}
void Board::makeMove(char* moves){
    Move temp_move;
    populateMove(moves,&temp_move);
    implementMove(&temp_move);
}
void Board::populateMove(char* moves, Move* move){
    int from_col = moves[0]-'a';
    int from_row = 8 - atoi(&moves[1]);
    int to_col = moves[2]-'a';
    int to_row = 8 - atoi(&moves[3]);
    move->fromSquare = 8*from_row + from_col;
    move->toSquare = 8*to_row + to_col;
    move->castle = 0;
    if(move->fromSquare==60 && move->toSquare==62 && this->castling==1){
        move->castle = 1;
    }
    else if(move->fromSquare==60 && move->toSquare==58 && this->castling==2){
        move->castle = 2;
    }
    else if(move->fromSquare==4 && move->toSquare==6 && this->castling==4){
        move->castle = 4;
    }
    else if(move->fromSquare==4 && move->toSquare==2 && this->castling==8){
        move->castle = 8;
    }
    if(moves[4]=='q'){
        move->promotion = this->turn==1 ? QUEEN_W : QUEEN_B;
    }
    else if(moves[4]=='r'){
        move->promotion = this->turn==1 ? ROOK_W : ROOK_B;
    }
    else if(moves[4]=='n'){
        move->promotion = this->turn==1 ? KNIGHT_W : KNIGHT_B;
    }
    else if(moves[4]=='b'){
        move->promotion = this->turn == 1 ? BISHOP_W : BISHOP_B;
    }
    if(this->black_pawn & (1LL << move->fromSquare)){
        move->pieceType = PAWN_B;
    }
    else if(this->white_pawn & (1LL << move->fromSquare)){
        move->pieceType = PAWN_W;
    }
    else if(this->black_bishop & (1LL << move->fromSquare)){
        move->pieceType = BISHOP_B;
    }
    else if(this->white_bishop & (1LL << move->fromSquare)){
        move->pieceType = BISHOP_W;
    }
    else if(this->black_rook & (1LL << move->fromSquare)){
        move->pieceType = ROOK_B;
    }
    else if(this->white_rook & (1LL << move->fromSquare)){
        move->pieceType = ROOK_W;
    }
    else if(this->black_knight & (1LL << move->fromSquare)){
        move->pieceType = KNIGHT_B;
    }
    else if(this->white_knight & (1LL << move->fromSquare)){
        move->pieceType = KNIGHT_W;
    }
    else if(this->black_queen & (1LL << move->fromSquare)){
        move->pieceType = QUEEN_B;
    }
    else if(this->white_queen & (1LL << move->fromSquare)){
        move->pieceType = QUEEN_W;
    }
    else if(this->black_king & (1LL << move->fromSquare)){
        move->pieceType = KING_B;
    }
    else if(this->white_king & (1LL << move->fromSquare)){
        move->pieceType = KING_W;
    }
}
void Board::implementMove(Move* move){
    bool enPassant = move->toSquare == this->enPassantSq && (move->pieceType==PAWN_W || move->pieceType==PAWN_B);
    if(enPassant){
        this->makeEnPassantMove(move);
        return;
    }
    if(move->castle){
        this->makeCastleMove(move);
        return;
    }
    this->board_hash = this->zobrist_pieces_hash[piece_value_map[move->pieceType]][move->fromSquare];
    this->board_hash ^= this->zobrist_white_to_move;
    this->board_hash ^= this->zobrist_castling[this->castling];
    if(this->enPassantSq!=-1){
        this->board_hash ^= this->zobrist_en_passant[this->enPassantSq];
    }
    this->enPassantSq = -1;
    bool starterPawn = (this->turn==1 && (move->pieceType==PAWN_W) && (move->fromSquare>=40 && move->fromSquare<=55)) || (this->turn == -1 && (move->pieceType==PAWN_B) && (move->fromSquare>=8 && move->fromSquare<=23));
    if(starterPawn){
        int distance = std::abs(move->fromSquare - move->toSquare);
        if(distance==16){
            this->enPassantSq = this->turn==1 ? move->fromSquare - 8 : move->fromSquare + 8;
        }
    } 
    if(this->enPassantSq != -1){
        this->board_hash ^= this->zobrist_en_passant[this->enPassantSq];
    }
    BitBoard* pieceMoved = nullptr;
    if(move->pieceType==PAWN_B){
        pieceMoved = &this->black_pawn;
    }
    else if(move->pieceType==PAWN_W){
        pieceMoved = &this->white_pawn;
    }
    else if(move->pieceType==KNIGHT_B){
        pieceMoved = &this->black_knight;
    }
    else if(move->pieceType==KNIGHT_W){
        pieceMoved = &this->white_knight;
    }
    else if(move->pieceType==ROOK_B){
        pieceMoved = &this->black_rook;
    }
    else if(move->pieceType==ROOK_W){
        pieceMoved = &this->white_rook;
    }
    else if(move->pieceType==BISHOP_B){
        pieceMoved = &this->black_bishop;
    }
    else if(move->pieceType==BISHOP_W){
        pieceMoved = &this->white_bishop;
    }
    else if(move->pieceType==QUEEN_B){
        pieceMoved = &this->black_queen;
    }
    else if(move->pieceType==QUEEN_W){
        pieceMoved = &this->white_queen;
    }
    else if(move->pieceType==KING_B){
        pieceMoved = &this->black_king;
    }
    else if(move->pieceType==KING_W){
        pieceMoved = &this->white_king;
    }
    if(move->pieceType==KING_W || move->pieceType==KING_B){
        this->castling &= this->turn==1 ? 0b1100 : 0b0011;
    }
    if(move->pieceType==ROOK_W || move->pieceType==ROOK_B){
        if(this->turn==1 && move->fromSquare==56)   this->castling &= 0b1101;
        if(this->turn==1 && move->fromSquare==63)   this->castling &= 0b1110;
        if(this->turn==-1 && move->fromSquare==0)   this->castling &= 0b0111;
        if(this->turn==-1 && move->fromSquare==7)   this->castling &= 0b1011;
    }
    *pieceMoved ^= 1LL << move->fromSquare;
    if(move->promotion==QUEEN_B){
        BitBoard* target = &this->black_queen;
        *target |= 1LL << move->toSquare;
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[QUEEN_B]][move->toSquare];
    }
    else if(move->promotion==QUEEN_W){
        BitBoard* target = &this->white_queen;
        *target |= 1LL << move->toSquare;
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[QUEEN_W]][move->toSquare];
    }
    else if(move->promotion==ROOK_B){
        BitBoard* target = &this->black_rook;
        *target |= 1LL << move->toSquare;
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[ROOK_B]][move->toSquare];
    }
    else if(move->promotion==ROOK_W){
        BitBoard* target = &this->white_rook;
        *target |= 1LL << move->toSquare;
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[ROOK_W]][move->toSquare];
    }
    else if(move->promotion==KNIGHT_B){
        BitBoard* target = &this->black_knight;
        *target |= 1LL << move->toSquare;
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[KNIGHT_B]][move->toSquare];
    }
    else if(move->promotion==KNIGHT_W){
        BitBoard* target = &this->white_knight;
        *target |= 1LL << move->toSquare;
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[KNIGHT_W]][move->toSquare];
    }
    else if(move->promotion==BISHOP_B){
        BitBoard* target = &this->black_bishop;
        *target |= 1LL << move->toSquare;
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[BISHOP_B]][move->toSquare];
    }
    else if(move->promotion==BISHOP_W){
        BitBoard* target = &this->white_bishop;
        *target |= 1LL << move->toSquare;
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[BISHOP_W]][move->toSquare];
    }
    else{
        *pieceMoved |= 1LL << move->toSquare;
        if(*pieceMoved==this->white_king){
            this->whiteKingSq = move->toSquare;
        }
        else if(*pieceMoved==this->black_king){
            this->blackKingSq = move->toSquare;
        }
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[move->pieceType]][move->toSquare];
    }
    BitBoard opponentRook = this->turn==1 ? this->black_rook : this->white_rook;
    BitBoard* temp_bb = this->turn==1 ? &this->black_pawn : &this->white_pawn;
    if(*temp_bb & this->board_squares[move->toSquare]){
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[this->turn==1 ? PAWN_B : PAWN_W]][move->toSquare];
        compareTempBBWithRookAndCastleChanges(*temp_bb,opponentRook,move);
        *temp_bb ^= 1LL << move->toSquare;
    }
    temp_bb = this->turn==1 ? &this->black_knight : &this->white_knight;
    if(*temp_bb & this->board_squares[move->toSquare]){
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[this->turn==1 ? KNIGHT_B : KNIGHT_W]][move->toSquare];
        compareTempBBWithRookAndCastleChanges(*temp_bb,opponentRook,move);
        *temp_bb ^= 1LL << move->toSquare;
    }
    temp_bb = this->turn==1 ? &this->black_bishop : &this->white_bishop;
    if(*temp_bb & this->board_squares[move->toSquare]){
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[this->turn==1 ? BISHOP_B : BISHOP_W]][move->toSquare];
        compareTempBBWithRookAndCastleChanges(*temp_bb,opponentRook,move);
        *temp_bb ^= 1LL << move->toSquare;
    }
    temp_bb = this->turn==1 ? &this->black_rook : &this->white_rook;
    if(*temp_bb & this->board_squares[move->toSquare]){
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[this->turn==1 ? ROOK_B : ROOK_W]][move->toSquare];
        compareTempBBWithRookAndCastleChanges(*temp_bb,opponentRook,move);
        *temp_bb ^= 1LL << move->toSquare;
    }
    temp_bb = this->turn==1 ? &this->black_queen : &this->white_queen;
    if(*temp_bb & this->board_squares[move->toSquare]){
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[this->turn==1 ? QUEEN_B : QUEEN_W]][move->toSquare];
        compareTempBBWithRookAndCastleChanges(*temp_bb,opponentRook,move);
        *temp_bb ^= 1LL << move->toSquare;
    }
    temp_bb = this->turn==1 ? &this->black_king : &this->white_king;
    if(*temp_bb & this->board_squares[move->toSquare]){
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[this->turn==1 ? KING_B : KING_W]][move->toSquare];
        compareTempBBWithRookAndCastleChanges(*temp_bb,opponentRook,move);
        *temp_bb ^= 1LL << move->toSquare;
    }
    this->board_hash ^= this->zobrist_castling[this->castling];
    this->turn = -1*this->turn;
    this->generateOccupancyMask();
}
void Board::makeEnPassantMove(Move* move){
    int capturedSquare = this->enPassantSq + (this->turn==1 ?  8 : -8);
    this->board_hash ^= this->zobrist_en_passant[this->enPassantSq];
    this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[move->pieceType]][move->fromSquare];
    this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[move->pieceType]][move->toSquare];
    this->board_hash ^= this->zobrist_pieces_hash[this->turn==1 ? 1 : 0][capturedSquare];
    BitBoard* playerPawns = this->turn==1 ? &white_pawn : &black_pawn;
    BitBoard* opponentPawns = this->turn==1 ? &black_pawn : &white_pawn;
    *opponentPawns^=1LL<<capturedSquare;
    *playerPawns ^= 1LL<<move->fromSquare;
    *playerPawns |= 1LL<<move->toSquare;
    this->enPassantSq = -1;
    this->turn = -1*this->turn;
    this->generateOccupancyMask();
}
void Board::makeCastleMove(Move* move){
    if(move->castle==1){
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[KING_W]][60];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[KING_W]][62];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[ROOK_W]][63];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[ROOK_W]][61];
        this->white_king ^= 1LL << 60;
        this->white_king |= 1LL << 62;
        this->white_rook ^= 1LL<<63;
        this->white_rook |= 1LL << 61;
        this->whiteKingSq = 62;
    }
    else if(move->castle==2){
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[KING_W]][60];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[KING_W]][58];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[ROOK_W]][56];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[ROOK_W]][59];
        this->white_king ^= 1LL << 60;
        this->white_king |= 1LL << 58;
        this->white_rook ^= 1LL<<56;
        this->white_rook |= 1LL << 59;
        this->whiteKingSq = 58;

    }
    else if(move->castle==4){
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[KING_B]][4];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[KING_B]][6];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[ROOK_B]][7];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[ROOK_B]][5];
        this->black_king ^= 1LL << 4;
        this->black_king |= 1LL << 6;
        this->black_rook ^= 1LL << 7;
        this->black_rook |= 1LL << 5;
        this->blackKingSq = 6;
    }
    else if(move->castle==8){
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[KING_B]][4];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[KING_B]][2];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[ROOK_B]][0];
        this->board_hash ^= this->zobrist_pieces_hash[piece_value_map[ROOK_B]][3];
        this->black_king ^= 1LL << 4;
        this->black_king |= 1LL << 2;
        this->black_rook ^= 1LL << 0;
        this->black_rook |= 1LL << 3;
        this->blackKingSq = 2;
    }
    if(this->enPassantSq!=-1){
        this->board_hash ^= this->zobrist_en_passant[this->enPassantSq];
    }
    this->board_hash ^= this->zobrist_white_to_move;
    this->board_hash ^= this->zobrist_castling[this->castling];
    this->castling &= this->turn==1 ? 0b1100 : 0b0011;
    this->board_hash ^= this->zobrist_castling[this->castling];
    generateOccupancyMask();
    this->enPassantSq = -1;
    this->turn = -1* this->turn;
}
void Board::compareTempBBWithRookAndCastleChanges(BitBoard bb, BitBoard rook, Move* move){
    if(bb==rook){
        if(this->turn==1 && move->fromSquare==56)   this->castling &= 0b1101;
        if(this->turn==1 && move->fromSquare==63)   this->castling &= 0b1110;
        if(this->turn==-1 && move->fromSquare==0)   this->castling &= 0b0111;
        if(this->turn==-1 && move->fromSquare==7)   this->castling &= 0b1011;
    }
}
BitBoard Board::get_pawn_white_right(int sq){
    return moveEngine.get_pawn_white_right(sq);
}
BitBoard Board::get_pawn_white_left(int sq){
    return moveEngine.get_pawn_white_left(sq);
}
int Board::basic_evaluate(BitBoard white_pawns, BitBoard black_pawns,
    BitBoard white_knights, BitBoard black_knights,
    BitBoard white_bishops, BitBoard black_bishops,
    BitBoard white_rooks, BitBoard black_rooks,
    BitBoard white_queens, BitBoard black_queens){
        return evaluationEngine.basic_evaluate(white_pawn,black_pawn,white_knights,black_knights,white_bishops, black_bishops, white_rooks, black_rooks,white_queen, black_queen);
}
BitBoard Board::get_pawn_attacks(int sq, bool whiteToPlay){
    return moveEngine.get_pawn_attacks(sq,whiteToPlay);
}
BitBoard Board::get_knight_attacks(int sq){
    return moveEngine.get_knight_attacks(sq);
}
BitBoard Board::get_king_attacks(int square){
    return moveEngine.get_king_attacks(square);
}
BitBoard Board::get_bishop_attacks(int square, BitBoard occupied){
    return moveEngine.get_bishop_attacks(square,occupied);
}
BitBoard Board::get_rook_attacks(int square, BitBoard occupied){
    return moveEngine.get_rook_attacks(square,occupied);
}
BitBoard Board::get_queen_attacks(int square, BitBoard occupied){
    return moveEngine.get_queen_attacks(square,occupied);
}
