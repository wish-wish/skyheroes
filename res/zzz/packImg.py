import os
import sys

PACK_FOLDERS =              \
[                           \
    ["effect/button","button"],           \
    ["effect/select effect/x effect/luck","select"],           \
    ["fruit","fruit"],         \
    ["result/b","result"],         \
    ["result/m result/y","result_e"],         \
    ["bets ui1 scale","uipic"],      \
]
    
def pack(folder):    
    cmd = "TexturePacker %s --sheet ../../res/games/SGPD2/%s.png --data ../../res/games/SGPD2/%s.plist --format cocos2d "%(folder[0], folder[1], folder[1])
    print(cmd)
    os.system(cmd)    

if __name__ == '__main__':

    if len(sys.argv) != 2:
        for folder in PACK_FOLDERS:
            pack(folder)
    else:
        pack(sys.argv[1])

