# demo-cefore

this git repo hosts the artefacts for a research paper

### title

"*Efficient Pull-based Mobile Video Streaming leveraging In-Network Functions*"

### authors

Kazuhisa Matsuzono, Hitoshi Asaeda, Indukala Naladala and Thierry Turletti

### venue

submitted at Globecom'19 conference # demo-cefore



## The scenario
We implemented the proposed scheme using [Cefore](https://cefore.net/), an open source software  that enables CCN-like communications.

To evaluate our proposed mobile video streaming mechanism, we chose a vehicular scenario in which a mobile user (a video consumer in a vehicle) streams a video from the infrastructure through an hybrid wireless/wired network. Then we compare the performance obtained with two other streaming mechanisms: (1) a basic TCP streaming application, and (2) a classical CCN (Cefore-based) streaming application that uses only regular interests (RGI).

Our scenario also uses the [ns-3 network simulator](https://www.nsnam.org/) in emulator mode with [direct code execution (DCE)](https://www.nsnam.org/docs/dce/manual/html/index.html)on one of the R2lab nodes to emulate two mobile content consumers (cars) running real application software (such as Cefore and TCP streaming). A consumer (in France) is connected to a real LTE network and the Internet through a ns-3 WiFi link (between the car and the road side unit RSU) to play a live video service content (published in Japan for the paper).
Note that consumer adaptation mechanisms used to dynamically select the appropriate quality version are not included in the scenario to focus on the mobility and in-network functions. The publisher thus provides a video content with a single bitrate of about 1.0 Mbps.

### How to run the demo

**Prerequisites**: you should have an account and a reserved slice on R2lab and have installed a few software on your machine. 

* To sign up and reserve a slice, see [https://r2lab.inria.fr/tuto-010-registration.md]()   
* To install the few companion software, see [https://r2lab.inria.fr/tuto-030-nepi-ng-install.md]()



### Cefore streaming scenario 

We assume that your publisher host has the following public IP address: a.b.c.d

Run on your machine:

*  ./mosaic-cefore.py -P a.b.c.d -l

Then, after about 5mn, when the script invites you to do so, run on the publisher host:

* 



### TCP streaming scenario

* Run on your machine:

 ./mosaic-cefore.py -t -P a.b.c.d -l
