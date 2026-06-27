the moon is a lie.

c++20 and g++

built for learning computer networking. resources I used:
- Beej's guide to network programming
- learncpp.com
- tcp/ip illustrated vol 1.

no llms used for writing code (no surprise to anyone if you skim through the code). only for asking questions / requesting generalised examples.

now, onto the serious things:

<h1>Protocol (subject to change)</h1>

the client is peer2peer and every node acts both as a client and a server, threads are spawned for each case. the established network protocol is:
- A TCP connection is established with a peer directly via hostname/port.
- a signal is sent over the wire to indicate the incoming information (uint8_t)
- the listener will then ingest the incoming packet with knowledge of what it contains

there are obvious tradeoffs with this approach, such that there is a packet-esque network on TCP and that multiple signals are sent over the wire in a seemingly atomic transaction, however this is to ensure reliability. also, since stream connections are reliable, they will be read in-order and not waste memory ingesting bigger/smaller packets than expected.

the packet dispatch is mutually exclusive from the listener so every piece of traffic is mostly(?) not policed (this client was built to be as secure as possible, but im no expert). incoming connections will be checked for legitimacy based on their structure, and transactions (contracts) over the network are built to be public and transparent so they are not encrypted (subject to change based on if i feel it is needed or not).

the client's virtual machine is built to judge the legitimacy of contracts on the network (it doesnt really matter where it originates from - if it is signed by both parties and legitimate, all bets are off). contracts are checked for legitimacy over the entire network and illegitimate ones will be unilaterally rejected and are thus pointless to make. the client will only be built to send legitimate contracts only based on the state of the network.


<br/>

this will be added to as the project continues, at the moment this is mostly a journal to log design decisions so I have something to come back to.



