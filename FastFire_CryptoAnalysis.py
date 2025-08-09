
# FastFire analysis — engineering-grade, reproducible
# Author: ChatGPT (per user request). Mirrors C++ CustomBoxWithBitwise & Permutation.
from collections import Counter
import random

MASK64 = (1<<64)-1

def bnot(b): return 1 - (b & 1)
def band(a,b): return (a & 1) & (b & 1)
def bor(a,b):  return (a | b) & 1
def bxor(a,b): return (a ^ b) & 1

def bits8_to_list(x): return [(x>>i)&1 for i in range(8)]
def list_to_bits8(xs):
    out=0
    for i,b in enumerate(xs): out |= (b&1)<<i
    return out & 0xFF

def custom_box_bit_sliced_map_byte(u:int)->int:
    x = bits8_to_list(u)
    # Step 1
    x[1]^=x[4]; x[3]^=x[7]; x[2]^=x[0]; x[6]^=x[5]
    x[4]^=x[6]; x[7]^=x[1]; x[4]^=x[2]; x[2]^=x[3]
    # Step 2
    t0=bnot(band(x[1],x[6])); t1=bnot(bor(x[0],x[1])); t2=bnot(band(x[2],x[4])); t3=bnot(bor(x[4],x[1]))
    t4=bnot(band(x[3],x[5])); t5=bnot(bor(x[5],x[2])); t6=bnot(band(x[4],x[3])); t7=bnot(bor(x[6],x[7]))
    # Step 3
    x[2]^=t0; x[1]^=t1; x[3]^=t2; x[5]^=t3; x[7]^=t4; x[4]^=t5; x[6]^=t6; x[0]^=t7
    # Step 3-1
    x[0]^=x[1]; x[1]^=bnot(x[2]); x[2]^=x[3]; x[3]^=x[4]
    x[4]^=x[5]; x[5]^=x[6]; x[6]^=bnot(x[7]); x[7]^=x[0]
    return list_to_bits8(x)

def ddt_lat_balance_proper():
    S=[custom_box_bit_sliced_map_byte(u) for u in range(256)]
    unique_outputs=len(set(S))
    bit_ones=[sum((s>>i)&1 for s in S) for i in range(8)]
    # DDT
    DDT=[[0]*256 for _ in range(256)]
    for a in range(256):
        for x in range(256):
            DDT[a][ S[x]^S[x^a] ] += 1
    max_ddt=0
    for a in range(1,256):
        max_ddt=max(max_ddt, max(DDT[a]))
    # LAT
    max_lat_abs=0
    for a in range(256):
        for b in range(1,256):
            total=0
            for x in range(256):
                ax=bin(a & x).count("1") & 1
                bs=bin(b & S[x]).count("1") & 1
                total += 1 if (ax^bs)==0 else -1
            if not (a==0 and b==0):
                if abs(total)>max_lat_abs: max_lat_abs=abs(total)
    return dict(unique_outputs=unique_outputs, bit_ones=bit_ones,
                max_ddt_nontrivial=max_ddt, max_lat_abs_nontrivial=max_lat_abs)

ROUND_CONSTANTS = [
    0xe80ac7166cf28f32,0x8c4eea5619edf50e,0x9c59c393f8bc9f31,0x0bdf57c08b5d36ef,
    0xb890d67c306b27c7,0x01a54cd116627b85,0x401e07294ce8c8bf,0x61d65684a349baae,
    0x6a578466d51d25ee,0xaf10cfa7c6ac0dfa,0x178aaa79a01623fe,0xe0d6a6f52e58afab,
    0x0543d0ed6e9b4d29,0x01B70C8E97AD5F98,0x243F6A8885A308D3,0x9E3779B97F4A7C15,0xB7E151628AED2A6A
]

def rotl(x,r): r&=63; return ((x<<r)|(x>>(64-r))) & MASK64
def rotr(x,r): r&=63; return ((x>>r)|(x<<(64-r))) & MASK64

def custom_box_bitwise_words(x):
    x0,x1,x2,x3,x4,x5,x6,x7 = x
    x1^=x4; x3^=x7; x2^=x0; x6^=x5; x4^=x6; x7^=x1; x4^=x2; x2^=x3
    t0=~(x1&x6)&MASK64; t1=~(x0|x1)&MASK64; t2=~(x2&x4)&MASK64; t3=~(x4|x1)&MASK64
    t4=~(x3&x5)&MASK64; t5=~(x5|x2)&MASK64; t6=~(x4&x3)&MASK64; t7=~(x6|x7)&MASK64
    x2^=t0; x1^=t1; x3^=t2; x5^=t3; x7^=t4; x4^=t5; x6^=t6; x0^=t7
    x0^=x1; x1^=(~x2)&MASK64; x2^=x3; x3^=x4; x4^=x5; x5^=x6; x6^=(~x7)&MASK64; x7^=x0
    return [x0,x1,x2,x3,x4,x5,x6,x7]

def permutation(state, rounds):
    st=state[:]
    for r in range(rounds):
        st[8]^=ROUND_CONSTANTS[r%len(ROUND_CONSTANTS)]
        st[:8]=custom_box_bitwise_words(st[:8])
        st[0]^=rotl(st[0],10)^rotl(st[0],37)
        st[1]^=rotr(st[1],53)^rotr(st[1],26)
        st[2]^=rotl(st[2],13)^rotr(st[2],6)
        st[3]^=rotr(st[3],50)^rotl(st[3],57)
        st[4]^=rotl(st[4],19)^rotl(st[4],9)
        st[5]^=rotr(st[5],25)^rotr(st[5],44)
        st[6]^=rotl(st[6],17)^rotr(st[6],8)
        st[7]^=rotr(st[7],46)^rotl(st[7],55)
        st[8]^=rotl(st[8],59)^rotl(st[8],29)
        st[9]^=rotr(st[9],25)^rotr(st[9],51)
        st[10]^=rotl(st[10],7)^rotr(st[10],15)
        st[11]^=rotr(st[11],35)^rotl(st[11],6)
        st[12]^=rotl(st[12],61)^rotl(st[12],30)
        st[13]^=rotr(st[13],29)^rotr(st[13],14)
        st[14]^=rotl(st[14],41)^rotr(st[14],20)
        st[15]^=rotr(st[15],22)^rotl(st[15],43)
        st[16]^=rotr(st[16],38)^rotl(st[16],13)
        st = st[1:]+st[:1]
    return st

def mc_differential(rounds, samples=4000, seed=0xC0FFEE):
    random.seed(seed)
    hw_out=[]
    for _ in range(samples):
        S=[random.getrandbits(64) for _ in range(17)]
        bitpos=random.randrange(64)
        Sin=S[:]; Sin[0]^=(1<<bitpos)
        Sout=permutation(S,rounds)
        Sout2=permutation(Sin,rounds)
        delta=[(a^b)&MASK64 for a,b in zip(Sout,Sout2)]
        hw_out.append(sum(x.bit_count() for x in delta))
    return dict(rounds=rounds, samples=samples,
                avg_total_hw=sum(hw_out)/len(hw_out),
                min_total_hw=min(hw_out), max_total_hw=max(hw_out))

def parity(x): return x.bit_count()&1
def mc_linear(rounds, samples=1000, mask_trials=32, seed=0xBADF00D):
    random.seed(seed)
    def rand_mask(): return [random.getrandbits(64) for _ in range(17)]
    best={"abs_corr":0.0}
    for _ in range(mask_trials):
        a=rand_mask(); b=rand_mask()
        agree=0
        for __ in range(samples):
            S=[random.getrandbits(64) for _ in range(17)]
            pa=0
            for i in range(17): pa ^= parity(a[i]&S[i])
            Sout=permutation(S,rounds)
            pb=0
            for i in range(17): pb ^= parity(b[i]&Sout[i])
            agree += (pa^pb)==0
        corr=(agree-(samples-agree))/samples
        if abs(corr)>best["abs_corr"]: best={"abs_corr":abs(corr)}
    return dict(rounds=rounds, mask_trials=mask_trials, samples_per_mask=samples, max_abs_corr=best["abs_corr"])

if __name__=="__main__":
    local = ddt_lat_balance_proper()
    print("Local mapping:", local)
    for r in [1,2,4,9]:
        print("MC diff", mc_differential(r))
    for r in [1,2,4,9]:
        print("MC lin ", mc_linear(r))
