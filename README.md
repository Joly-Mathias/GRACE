# GRACE
Research internship project in Cryptography on Private Information Retrieval

## Code

### GMP Library
download the GMP library from https://gmplib.org/  
./configure --enable-cxx  
make  
make check  
sudo make install  

download the library libgmpxx  
export /usr/local/lib  
compile with additionnal libraries : -lgmpxx -lgmp  

## Bibliography

### Already read
N. Gilboa and Y. Yshai (2014). Distributed Point Functions and Their Applications. In: Nguyen, P.Q., Oswald, E. (eds) Advances in Cryptology – EUROCRYPT 2014. EUROCRYPT 2014. Lecture Notes in Computer Science, vol 8441. Springer, Berlin, Heidelberg. https://doi.org/10.1007/978-3-642-55220-5_35

F. Wang, C. Yun, S. Goldwasser, V. Vaikuntanathan and M. Zaharia (2017). Splinter: Practical Private Queries on Public Data. In 14th USENIX Symposium on Networked Systems Design and Implementation (NSDI 17). Boston, MA, 299-313. https://www.usenix.org/conference/nsdi17/technical-sessions/presentation/wang-frank

E. Boyle, N. Gilboa and Y. Ishai (2015). Function Secret Sharing. In: Oswald, E., Fischlin, M. (eds) Advances in Cryptology - EUROCRYPT 2015. EUROCRYPT 2015. Lecture Notes in Computer Science(), vol 9057. Springer, Berlin, Heidelberg. https://doi.org/10.1007/978-3-662-46803-6_12

E. Boyle, N. Gilboa, and Y. Ishai (2016). Function Secret Sharing: Improvements and Extensions. In Proceedings of the 2016 ACM SIGSAC Conference on Computer and Communications Security (CCS '16). Association for Computing Machinery, New York, NY, USA, 1292–1303. https://doi.org/10.1145/2976749.2978429

D.Kogan and H. Corrigan-Gibbs (2021). Private Blocklist Lookups with Checklist. In 30th USENIX Security Symposium (USENIX Security 21) https://www.usenix.org/conference/usenixsecurity21/presentation/kogan

B. Chor, O. Goldreich, E. Kushilevitz and M. Sudan (1995). Private Information Retrieval. Proc. of 36th FOCS, pp. 41-50. https://madhu.seas.harvard.edu/papers/1995/pir-journ.pdf

B. Chor and N. Gilboa (1997). Computationally private information retrieval (extended abstract). In Proceedings of the twenty-ninth annual ACM symposium on Theory of computing (STOC '97). Association for Computing Machinery, New York, NY, USA, 304–313. https://doi.org/10.1145/258533.258609

A. Beimel, Y. Ishai, T. Malkin (2000). Reducing the Servers Computation in Private Information Retrieval: PIR with Preprocessing. In: Bellare, M. (eds) Advances in Cryptology — CRYPTO 2000. CRYPTO 2000. Lecture Notes in Computer Science, vol 1880. Springer, Berlin, Heidelberg. https://doi.org/10.1007/3-540-44598-6_4

H. Corrigan-Gibbs, D. Kogan (2020). Private Information Retrieval with Sublinear Online Time. In: Canteaut, A., Ishai, Y. (eds) Advances in Cryptology – EUROCRYPT 2020. EUROCRYPT 2020. Lecture Notes in Computer Science(), vol 12105. Springer, Cham. https://doi.org/10.1007/978-3-030-45721-1_3

E. Boyle, N. Gilboa and Y. Ishai (2019). Secure Computation with Preprocessing via Function Secret Sharing. In: Cryptology ePrint Archive, Report 2019/1095. https://ia.cr/2019/1095

E. Stefanov, M. van Dijk, E. Shi, T-H. H. Chan, C. Fletcher, L. Ren. X. Yu. S. Devadas (2013). Path ORAM : An extremely Simple Oblivious RAM Protocol

O. Goldreich and R. Ostrovsky. Software Protection and Simulation on Oblivious RAMs. Journal of the ACM, 1996

A. Hamlin and M. Varia (2020). Two-server Distributed ORAM with Sublinear Computation and Constant Rounds. Cryptology ePrint Archive

B. Chor, N. Gilboa, and M. Naor (1997). Private Information Retrieval by Keywords.TR CS0917, Department of Computer Science, Technion https://www.cs.bgu.ac.il/~gilboan/publications/PERKY.pdf


### Reading

A. Beimel and Y. Ishai (2001) Information-theoretic private information retrieval: a unified construction. In P. G. Spirakis and J. van Leeuven, editors, Proc. of the 28th Internat. Colloq. on Automata, Languages and Programming, volume 2076 of Lecture Notes in Computer Science, pages 912–926. Springer-Verlag, Berlin, https://doi.org/10.1007/3-540-48224-5_74

### To read

#### Oblivious RAM

J. Doerner and A. Shelat (2017) Scaling ORAM for Secure Computation. Cryptology ePrint Archive

M.T. Goodrich and M. Mitzenmacher. Privacy-preserving access of out-sourced data via oblivious RAM simulation. Proceedings of ICALP 2011

B. Pinkas ans T. Reinman. Oblivious RAM revisited. Crypto 2010

E. Shi, T-H H. Chan, E. Stefanov and M Li. Oblivious RAM with O((log N)^3 ) worst-case cost. Proceedings of Asiacrypt 2011, pp.197-214, 2011

#### PIR

A. Beimel, Y. Ishai and T. Malkin (2004). Reducing the servers’ computation in Private Information Retrieval : PIR with preprocessing. Journal of Cryptology, 17(2), pp.125-151, 2004.

C. Devet, I. Goldberg and N. Heninger (2012). Optimally Robust Private Information Retrieval. 21st USENIX Security symposium, 2012

C. Devet and I. Goldberg (2014). The best of both worlds : combining IT- and C-PIR for communication efficiency. Privacy Enhanced Technologies (PETs) 2014.

A. Ambainis (1996). Upper bound on the communication complexity of private information retrieval. In: Degano, P., Gorrieri, R., Marchetti-Spaccamela, A. (eds) Automata, Languages and Programming. ICALP 1997. Lecture Notes in Computer Science, vol 1256. Springer, Berlin, Heidelberg. https://doi.org/10.1007/3-540-63165-8_196

Y. Gertner, Y. Ishai, E. Kushilevitz, and T. Malkin (1997). Protecting Data Privacy in
Private Information Retrieval Schemes. In Proceedings of the thirtieth annual ACM symposium on Theory of computing (STOC '98). Association for Computing Machinery, New York, NY, USA, 151–160. DOI:https://doi.org/10.1145/276698.276723

F. Olumofin and I. Goldberg (2011). Revisiting the computational practicality of private information retrieval. In Financial Cryptography and Data Security, pages 158–172. https://doi.org/10.1007/978-3-642-27576-0_13

R. Ostrovsky and V. Shoup (1996). Private information storage (STOC '97). pp. 294-303. ACM. https://doi.org/10.1145/258533.258606

D. Kales, O. Omolola, S. Ramacher (2019).Revisting User Privacy for Certificate Transparency.EuroS&P 2019

G. Di Crescenzo, Y. Ishai, and R. Ostrovsky (2001).Universal service-providers for private information retrieval. J. Cryptol., 14(1):37–74.

S. Patel, G. Persiano, and K. Yeo (2018). Private stateful information retrieval. CCS 2018.

 E. Boyle, Y. Ishai, R. Pass, and M. Wootters. Can we access a database
both locally and privately? TCC 2017.

Z. Dvir and S. Gopi. 2-server PIR with subpolynomial communication. J. ACM, 63(4):39:1–39:15, 2016.

#### Codes correcteurs

Swastik Kopparty, Shubangi Saraf and Sergey Yekhanin. High-Rate Codes with Sublinear-Time Decoding. In STOC 2011, Proceedings of the Fourty-third Annual ACM Symposium on Theory of Computing, 2011.

Jonathan Katz and Luca Trevisan. On the efficiency of local decoding procedures for error-correcting codes. In STOC 2000, Proceedings of the Thirty Second Annual ACM Symposium on Theory of Computing, 2000.

. Augot, F. Levy-dit-Vehel, A. Shikfa. “A Storage-efficient and Robust Private Information Retrieval Scheme allowing few servers”. Proceedings of 13th International Conference on Cryptography and Network Security, CANS’2014, Springer vol.8813 pp.222–239, 2014.