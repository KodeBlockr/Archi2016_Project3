0x00000008
      . lw   $2 0($0)
      . lw   $3 4($0)
      . xor  $8 ,$2 ,$3
      . add  $2 ,$2 ,$3
      . nand $10,$2 ,$3
      . slt  $14,$12 ,$2
      . add  $2 ,$2 ,$3
      . sub  $2 ,$2 ,$3
      . and  $6 ,$2 ,$3
      . or   $7 ,$5 ,$8
      . nor  $9 ,$2 ,$3
      . nand $10,$2 ,$3
      . srl  $3 $12 12
      . slt  $14,$12 ,$2
      . sra  $2 $3 12
      . addi  $2 ,$2 ,123
      . sw   $2 12($14)
      . beq  $2 $14 PPP
   GGG  addi $0  $4 0
      . j KKK
   PPP  bne  $13 $15 GGG
   KKK  addi $4 $4 -323
      . sh   $9 12($16)
      . lw   $9 12($0)
   AAA  halt
      . halt
      . halt
      . halt
      . halt
      . srl  $3 $12 12
      . srl  $2 $12 12
      . sra  $2 $3 12
      . sra  $3 $2 12
      . addi  $2 ,$2 ,123
      . beq  $2 $14 PPP
   GGG  addi $0  $4 0
      . j KKK
   PPP  bne  $13 $15 GGG
   KKK  addi $4 $4 -323
      . sh   $9 12($16)
      . lw   $9 12($0)
   AAA  halt
      . halt
      . halt
      . halt
      . halt