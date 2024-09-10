LEAHU MORIE ROBERT ANDREI 321 CB

Am folosit ca schelet pentru aceasta tema laboratorul 8 si l-am modificat putin ca sa ma pliez pe
cerintele temei ,iar ca limbaj de programare am ales sa fac in c++
Primul lucru pe care il fac este sa creez si initializez soketii tcp si udp
Dupa completez adresa serverului addr_tcp pentru  bind apoi asociem fiecare socket UDP si TCP cu portul ales,iar 
apoi fac starea descriptorilor socketilor
Mi am facut o structura client unde am id,topics,connected(variabila care verifica daca m am conectat) si socket
In server iau fiecare caz in parte :
|- prima data verific dsaca este egal cu socket ul de udp
|-dupa se primeste mesajul de la clientul udp 
|-copiez primele 50 elem intr un nou buffer ,deoarece vreau sa extrag  din buff topic, datatype , mesaj si continut 
|-dupa caut in vectorul de clientii si apoi in vectorul de topicuri , iar daca am gasit topicul respectiv
il trimit cu ajutorul functiei send

i == sock_tcp
|- se primeste mesajul la clientul TCP
|-parcurg din nou clientii si verific daca am gasit clientul cu acelasi id , iar daca da afisez mesajul ca exista deja 
|-mi am luat o variabila ok in care verific daca exista un client conectat sau nu ,iar daca 
ok = 0 atunci adaug clientul nou 

i == 0
|-daca dau comanda de la tastatura "exit" clientii se deconecteaza 

Dupa iau fiecare subscriber si il deconectez de la server dupa aplicarea comenzii "exit", astfel incat 
dupa afisarea mesajului clientul practic este eliminat din vector
Apoi am incercat sa fac partea de subscribe si unsubscribe  si am reusit pana la urma sa dau subscribe cu topicul 
si am rulat sa ma verific cu udp_client.py
Am gestionat posibile erori folosind functia DIE, din scheletul de laborator.
La final, inchid socketii activi.

La SUBSCRIBER
|-declar variabilele de care am nevoie 
|-golesc multimile de descriptori utilizate
|-dupa incep sa verific daca este rulat corect executabilul 
|-mai apoi creez socketul si dezactivez algoritmul lui Nagle
|-conectez clientul la server
|-trimit idiul serverului
dupa cat timp e true
|- aleg un socket pentru interactiune
|- verific daca socketul apartine multimii tmp_fds si dupa citim de la tastatura cpmenziile
|- apoi mai verific daca socketul apartine multimii tmp_fds inca o data ca sa primesc structura de la server
si apoi verific numarul de bytes pe care ii primesc
|-iar la final de tot inchid socketul