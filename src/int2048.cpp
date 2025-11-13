#include "int2048.h"
#include <csignal>

namespace sjtu {
    const int kMod1 = 998244353 , kG = 3 , kBase = 1e9 , kDigit = 9 ;
    const int kMod2 = 1004535809 , kMod3 = 469762049 ;
    const long long kM = 1LL * kMod1 * kMod2 ;
    int r[kMaxn] , lim = 0 ;
    __int128 ans[kMaxn] ;
    int2048::int2048 () {
        //std::cout << "Error\n" ;
        a.resize (1) ;
        len = 1 ; sgn = 1 ; a[0] = 0 ;
    }
    int2048::int2048 (long long x) {
        len = 0 ; sgn = 1 ; a.clear () ;
        if (x < 0) sgn = -1 , x = -x ;
        if (x == 0) {
            len = 1 ; a.resize (1) ;
            a[0] = 0 ;
        }
        //std::cout << "*" ;
        while (x) {
            a.push_back (x % kBase) ;
            x /= kBase ;
            //std::cout << x << '\n' ;
        }
        len = a.size () ;
    }
    int2048::int2048 (const std::string &s) {
        int sz = s.size () ; a.clear () ;
        if (s[0] == '-') {
            sgn = -1 ;
            for (int i = sz - 1 ; i >= 1 ; i -= kDigit) {
                int x = 0 ;
                for (int j = std::max (1 , i - kDigit + 1) ; j <= i ; j++)
                    x = x * 10 + s[j] - '0' ;
                a.push_back (x) ;
            }
        }
        else {
            sgn = 1 ;
            for (int i = sz - 1 ; i >= 0 ; i -= kDigit) {
                int x = 0 ;
                for (int j = std::max (0 , i - kDigit + 1) ; j <= i ; j++)
                    x = x * 10 + s[j] - '0' ;
                a.push_back (x) ;
            }
        }
        len = a.size () ;
        //print () ;
        //std::cout << '\n' ;
        if (len == 1 && a[0] == 0 && sgn == -1) sgn = 1 ;
        //std::cout << a.size () << ' ' << len << '\n' ; 
    }
    int2048::int2048 (const int2048 &x) {
        len = x.len ; a.resize (len) ;
        sgn = x.sgn ;
        for (int i = 0 ; i < len ; i++) a[i] = x.a[i] ;
    }
    void int2048::read (const std::string &s) {
        (*this) = int2048 (s) ;
        //std::cout << (*this).a.size () << '\n' ;
        //std::cout << "asdf"<<(*this).sgn << '\n' ;
    }
    void write (int x , int dep , std::ostream & out) {
        if (!dep) return ;
        write (x / 10 , dep - 1 , out) ;
        out << x % 10 ;
    }
    void int2048::print () {
        if (sgn == -1) std::cout << '-' ;
        for (int i = a.size () - 1 ; i >= 0 ; i--) {
            if (i == a.size () - 1) std::cout << a[i] ;
            else write (a[i] , kDigit , std::cout) ;
        }
    }
    int2048 &int2048::add (const int2048 &x) {
        if (this == &x) {
            int2048 tmp = x ;
            return add (tmp) ;
        }
        //std::cout << sgn << ' ' << x.sgn << '\n' ;
        // std::cout << '@' << len << ' ' << x.len << '\n' ;
        // for (int i = a.size () - 1 ; i >= 0 ; i--) std::cout << a[i] ;
        // puts ("-") ;
        // for (int i = x.a.size () - 1 ; i >= 0 ; i--) std::cout << x.a[i] ;
        // puts ("-") ;
        if (sgn != x.sgn) {
            //std::cout << "Error\n" ;
            int2048 tmp = x ; tmp.sgn = -tmp.sgn ;
            return minus (tmp) ;
        }
        //std::cout << a.size () << '\n' ;
        len = std::max (len , x.len) + 1 ;
        //std::cout << '@' << len << ' ' << x.len << '\n' ;
        a.resize (len) ;
        for (int i = 0 ; i < len ; i++) {
            //std::cout << "<*" << a[i] << ' ' << x.a[i] << '\n' ;
            if (i < x.len) a[i] += x.a[i] ;
            if (i < len - 1 && a[i] >= kBase) a[i + 1]++ , a[i] -= kBase ;
            //std::cout << '*' << a[i] << '\n' ;
        }
        //print () ; std::cout << '\n' ;
        while (len > 1 && a[len - 1] == 0) len-- ;
        a.resize (len) ;
        if (len == 1 && a[0] == 0 && sgn == -1) sgn = 1 ;
        //print () ; puts ("---") ;
        return *this ;
    }
    int2048 add (int2048 x , const int2048 &y) {
        //std::cout << '*' << x.sgn << '\n' ;
        //x.print () ; std::cout << '\n' ;
        x.add (y) ;
        return x ;
    }
    int2048 &int2048::minus (const int2048 &x) {
        if (this == &x) {
            int2048 tmp = x ;
            return minus (tmp) ;
        }
        if (sgn != x.sgn) {
            int2048 tmp = x ; tmp.sgn = -tmp.sgn ;
            return add (tmp) ;
        }
        if (sgn == -1) {
            if (x < (*this)) {//|this| < |x|
                int2048 tmp = x ;
                tmp.sgn = sgn = 1 ;
                (*this) = tmp.minus ((*this)) ;
                return (*this) ;
            }
        }
        else {
            if ((*this) < x) {//|this| < |x|
                int2048 tmp = x ;
                tmp.sgn = sgn = -1 ;
                (*this) = tmp.minus ((*this)) ;
                return (*this) ;
            }
        }
        len = std::max (len , x.len) ;
        a.resize (len) ;
        for (int i = 0 ; i < len ; i++) {
            if (i < x.len) a[i] -= x.a[i] ;
            if (a[i] < 0) a[i] += kBase , a[i + 1]-- ;
        }
        while (len > 1 && a[len - 1] == 0) len-- ;
        a.resize (len) ;
        if (sgn == -1 && len == 1 && a[0] == 0) sgn = 1 ;
        return (*this) ;
    }
    int2048 minus (int2048 x , const int2048 &y) {
        return x.minus (y) ;
    }
    bool operator < (const int2048 &x , const int2048 &y) {
        if (x.sgn != y.sgn) return x.sgn < y.sgn ;
        if (x.len != y.len) {
            return (x.sgn == 1) ? (x.len < y.len) : (x.len > y.len) ;
        }
        for (int i = x.len - 1 ; i >= 0 ; i--) {
            if (x.a[i] < y.a[i]) return (x.sgn == 1) ;
            if (x.a[i] > y.a[i]) return (x.sgn == -1) ;
        }
        return 0 ;
    }
    bool operator > (const int2048 &x , const int2048 &y) {
        return y < x ;
    }
    bool operator == (const int2048 &x , const int2048 &y) {
        if (x.sgn != y.sgn) return 0 ;
        if (x.len != y.len) return 0 ;
        for (int i = 0 ; i < x.len ; i++)
            if (x.a[i] != y.a[i]) return 0 ;
        return 1 ;
    }
    bool operator <= (const int2048 &x , const int2048 &y) {
        return x < y || x == y ;
    }
    bool operator >= (const int2048 &x , const int2048 &y) {
        return x > y || x == y ;
    }
    bool operator != (const int2048 &x , const int2048 &y) {
        return !(x == y) ;
    }
    int2048 &int2048::operator = (const int2048 &x) {
        if (this == &x) return (*this) ; 
        sgn = x.sgn ; 
        len = x.len ;
        a.resize (len) ;
        for (int i = 0 ; i < len ; i++) a[i] = x.a[i] ;
        //std::cout << sgn << '\n' ;
        return (*this) ;
    }
    int2048 operator + (int2048 x , const int2048 &y) {
        return x.add (y) ;
    }
    int2048 operator - (int2048 x , const int2048 &y) {
        return x.minus (y) ;
    }
    int2048 &int2048::operator += (const int2048 &x) {
        return add (x) ;
    }
    int2048 &int2048::operator -= (const int2048 &x) {
        return minus (x) ;
    }
    std::istream &operator >> (std::istream &in , int2048 &x) {
        std::string s ; in >> s ;
        x = int2048 (s) ;
        return in ;
    }
    std::ostream &operator << (std::ostream &out , const int2048 &x) {
        if (x.sgn == -1) out << '-' ;
        for (int i = x.len - 1 ; i >= 0 ; i--) {
            if (i == x.len - 1) out << x.a[i] ;
            else write (x.a[i] , kDigit , out) ;
        }
        return out ;
    }
    long long qpow (long long x , long long p , long long Mod) {
        long long ret = 1 ;
        for (; p ; p >>= 1 , x = x * x % Mod)
            if (p & 1) ret = ret * x % Mod ;
        return ret ;
    }
    void NTT (std::vector <int> &t , int k , int Mod) {
        int Gi = qpow (kG , Mod - 2 , Mod) ;
        t.resize (lim) ;
        for (int i = 0 ; i < lim ; i++)
		    if (i > r[i]) std::swap (t[i] , t[r[i]]) ;
	    for (int mid = 1 ; mid < lim ; mid <<= 1) {
            long long w = qpow (k == 1 ? kG : Gi , (Mod - 1) / (mid << 1) , Mod) ;
            for (int nw = 0 ; nw < lim ; nw += (mid << 1)) {
                long long p = 1 ;
                for (int j = 0 ; j < mid ; j++ , p = (p * w) % Mod) {
                    long long x = t[j + nw] , y = p * t[j + nw + mid] % Mod ;
                    t[j + nw] = (x + y) % Mod ;
                    t[j + nw + mid] = (x - y + Mod) % Mod ;
                }
            }
        }
        if (k == 1) return ;
        long long inv = qpow (lim , Mod - 2 , Mod) ;
        for (int i = 0 ; i < lim ; i++) t[i] = t[i] * inv % Mod ;
    }
    int int2048::size () {return len ;}
    void Transform (std::vector <int> x , std::vector <int> y , std::vector <int> &ret , long long Mod) {
        NTT (x , 1 , Mod) , NTT (y , 1 , Mod) ;
        ret.resize (lim) ;
        for (int i = 0 ; i < lim ; i++) ret[i] = (long long)x[i] * y[i] % Mod ;
        NTT (ret , -1 , Mod) ;
    }
    int2048 operator * (int2048 x , const int2048 &y) {
        int2048 ret ; ret.len = x.len + y.len + 1 ;
        ret.sgn = (x.sgn == y.sgn) ? 1 : -1 ;
        int2048 ret2 = ret , ret3 = ret ;
        int cnt = 0 ;
        for (lim = 1 ; lim <= ret.len ; lim <<= 1 , cnt++) ;
        for (int i = 0 ; i < lim ; i++)
            r[i] = (r[i >> 1] >> 1) | ((i & 1) << (cnt - 1)) ;
        Transform (x.a , y.a , ret.a , kMod1) ;
        Transform (x.a , y.a , ret2.a , kMod2) ;
        Transform (x.a , y.a , ret3.a , kMod3) ;
        ret.a.resize (ret.len) ;
        ret2.a.resize (ret.len) ;
        ret3.a.resize (ret.len) ;
        for (int i = 0 ; i < ret.len ; i++) {
            __int128 A ;
            A = (__int128)ret.a[i] * kMod2 % kM * qpow (kMod2 % kMod1 , kMod1 - 2 , kMod1) % kM
              + (__int128)ret2.a[i] * kMod1 % kM * qpow (kMod1 % kMod2 , kMod2 - 2 , kMod2) % kM ;
            if (A >= kM) A -= kM ;
            __int128 K = ((ret3.a[i] - A) % kMod3 + kMod3) % kMod3 * qpow (kM % kMod3 , kMod3 - 2 , kMod3) % kMod3 ;
            ans[i] = K * kM + A ;
        }
        for (int i = 0 ; i < ret.len ; i++) {
            if (i < ret.len - 1) ans[i + 1] += ans[i] / kBase ;
            ans[i] %= kBase ;
        }
        for (int i = 0 ; i < ret.len ; i++) ret.a[i] = ans[i] ;
        while (ret.len > 1 && ret.a[ret.len - 1] == 0) ret.len-- ;
        ret.a.resize (ret.len) ;
        if (ret.len == 1 && ret.a[0] == 0 && ret.sgn == -1) ret.sgn = 1 ;
        return ret ;
    }
    int2048 &int2048::operator *= (const int2048 &y) {
        (*this) = (*this) * y ;
        return (*this) ;
    }
    int popcount (int x) {
        if (x > 0 && x < 10) return 1 ;
        if (x == 0) return 1 ;
        return popcount (x / 10) + 1 ;
    }
    int popcount (int2048 x) {
        return (x.len - 1) * kDigit + popcount (x.a[x.len - 1]) ;
    }
    void move (int2048 &x , int k) {
        if (!k) return ;
        if (k > 0) {
            x.a.resize (x.len + k) ;
            for (int i = x.len - 1 ; i >= 0 ; i--)
                x.a[i + k] = x.a[i] ;
            for (int i = 0 ; i < k ; i++) x.a[i] = 0 ;
            x.len += k ;
        }
        else {
            k = -k ;
            for (int i = 0 ; i < x.len - k ; i++)
                x.a[i] = x.a[i + k] ;
            x.len -= k ;
            x.a.resize (x.len) ;
        }
    }
    int2048 forcediv (int2048 x , int2048 y) {
        int2048 t1 = y , t2 = 1 , t3 , ans = 0 ;
        std::vector <int2048> v1 , v2;
        while (t1 <= x) {
            t3 = t1 + t1 ;
            t1 = t3 ;
            t3 = t2 + t2 ;
            t2 = t3 ;
            v1.push_back (t1) ;
            v2.push_back (t2) ;
        }
        for (int i = v1.size () - 1 ; i >= 0 ; i--) {
            if (v1[i] <= x) {
                x -= v1[i] ;
                ans += v2[i] ;
            }
        }
        return ans ;
    }
    int2048 getinv (int2048 x) {
        if (x.len <= 5) {
            int2048 tmp ;
            tmp.a.resize (x.len * 2 + 1) ;
            tmp.len = tmp.a.size () ;
            tmp.a[tmp.len - 1] = 1 ;
            return forcediv (tmp , x) ;
        }
        //x_k+1=x_k*(2-B*x_k)
        int2048 t1 , t2 , t3 ;
        int m = x.len , n = (x.len + 5) / 2 ;
        t1.len = n ; t1.a.resize (n) ;
        for (int i = n - 1 , j = m - 1 ; i >= 0 && j >= 0 ; i-- , j--)
            t1.a[i] = x.a[j] ;
        int2048 ans = 0 ;
        t2 = getinv (t1) ;//x_k
        t1 = t2 * x ;//b * x_k
        t3 = t1 * t2 ;//x_k * b * x_k ;
        move (t3 , -2 * n) ;
        t1 = t2 + t2 ;//2 * x_k
        move (t1 , m - n) ;
        ans = t1 - t3 ;//2 * x_k - x_k * x_k * B
        ans = ans - 1 ;//adjustment
        t3.a.clear () ; t3.a.resize (2 * m + 1) ; t3.len = 2 * m + 1 ;
        t3.a[t3.len - 1] = 1 ;
        t2 = t3 - t1 ;
        if (t2 >= x) ans = ans + 1 ;
        return ans ;
    }
    int2048 operator / (int2048 x , const int2048 &y) {
        if (x == 0) return 0 ;
        if (y == 1) return x ;
        if (y == -1) return -x ;
        int2048 absx = x , absy = y ;
        absx.sgn = absy.sgn = 1 ;
        if (absx < absy) {
            if (x.sgn == y.sgn) return 0 ;
            return -1;
        }
        int n = absx.len , m = absy.len ;
        if (n > 2 * m) {
            move (absx , n - 2 * m) ;
            move (absy , n - 2 * m) ;
            m = n - m ;
            n = 2 * m ;
        }
        int2048 t1 = absx , t2 = absy , t3 ;
        t3 = getinv (absy) ;
        t2 = t1 * t3 ;
        move (t2 , -2 * m) ;
        if (x.sgn == y.sgn) {
            t1 = t2 * absy ;
            t3 = absx - t1 ;
            if (t3 >= absy) t2 = t2 + 1 ;
            else if (t3 < 0) t2 = t2 - 1 ;
        }
        else {
            t1 = t2 * absy ;
            t3 = absx - t1 ;
            if (t3 > 0) t2 = t2 + 1 ;
            else if (t3 <= -absy) t2 = t2 - 1 ;
        }
        t2.sgn = (x.sgn == y.sgn ? 1 : -1) ;
        return t2 ;
    }
    int2048 &int2048::operator /= (const int2048 &y) {
        (*this) = (*this) / y ;
        return (*this) ;
    }
    int2048 operator % (int2048 x , const int2048 &y) {
        return x - (x / y) * y ;
    }
    int2048 &int2048::operator %= (const int2048 &y) {
        (*this) = (*this) % y ;
        return (*this) ;
    }
    int2048 int2048::operator + () const {
        return (*this) ;
    }
    int2048 int2048::operator - () const {
        if (len == 1 && a[0] == 0) return (*this) ;
        int2048 ret = (*this) ;
        ret.sgn = -ret.sgn ;
        return ret ;
    }
    long long transtoll (int2048 x) {
        long long ret = 0;
        for (int i = x.len - 1; i >= 0; i--)
            ret = ret * kBase + x.a[i];
        return ret;
    }
    std::string transtostring (int2048 x) {
        if (x == 0) return "0";
        std::string ret = "";
        if (x.sgn == -1) ret += '-';
        for (int i = x.len - 1; i >= 0; i--) {
            std::string tmp = ""; long long nw = x.a[i];
            if (i != x.len - 1) {
                for (int i = 0; i < kDigit; i++)
                    tmp = (char)(nw % 10 + '0') + tmp, nw /= 10;
            }
            else {
                while (nw) tmp = (char)(nw % 10 + '0') + tmp, nw /= 10;
            }
            ret += tmp;
        }
        return ret;
    }
} // namespace sjtu
