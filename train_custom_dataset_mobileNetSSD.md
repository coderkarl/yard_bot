# MobileNetSSD Custom Dataset instructions with caffe

## Label Images with LabelImg tool
github.com/tzutalin/labelimg 
I used the docker (currently on kxps laptop) 
Use Pascal VOC xml format for labels 
I used robag-to-jpg scripts to save images as frame0001.jpg etc. 
This gave a set of frame0001-0100.jpg for each bag subfolder. 
This was an issue because every image needs a unique filename. 
Before labeling, setup a unique filename standard. 
The Pascal VOC xml includes a file path, but I concluded the path is ignored. 
(I moved the subfolders to my workstation.) 

base_dir = /home/deek/MyDataset/ConeLabels 
base_dir/Images, Labels, Structure 
base_dir/labelmap.prototxt 
Structure is initially empty and will be filled by createTrainValTxt.py 

### Create train_val, test txt with create_data.sh
/home/deek/MyDataset/createTrainValTxt.py 
Tutorial suggests if less than 200 images, just copy trainval.txt to test.txt 
If you have enough images per label, manually move lines from trainval.txt to test.txt

## Caffe install from github repo
/home/deek/deep/ssd_caffe_ref/
git clone https://github.com/weiliu89/caffe.git
cd caffe/
git checkout ssd

### Build caffe prep
OpenCV (Does installing ROS-kinetic automatically provide OPENCV 3?)
Make sure OPENCV 3 version includes dnn
I only got the build to work with python 2.7

sudo apt-get install libprotobuf-dev libleveldb-dev libsnappy-dev libopencv-dev libhdf5-serial-dev protobuf-compiler
sudo apt-get install --no-install-recommends libboost-all-dev
sudo apt-get install libhdf5-dev
sudo apt-get install libhdf5_hl
sudo apt-get install libopenblas-dev
sudo apt-get install python-scikits-learn
sudo apt-get install python-skimage
sudo apt-get install python-protobuf

cd /usr/lib/x86_64-linux-gnu/
sudo ln -s libhdf5_serial.so libhdf5.so
sudo ln -s libhdf5_serial_hl.so.10.0.2 libhdf5_hl.so

??
export PYTHONPATH=/usr/lib/python2.7/:/home/deek/deep/ssd_caffe_ref/caffe/python
??

??
http://www.voidcn.com/article/p-qzjrzdra-bkq.html, modified opencv3_0_0.sh
Downloads/opencv_install.sh
This failed, but something may have installed enough to help the build
??


Build with make clean, sudo ldconfig, make -j4, make py, make test -j4

### docker from repo fails on my gpu
cd deep/ssd_caffe_ref/caffe/docker/
nvidia-docker build -t caffe:gpu standalone/gpu

## Create lmdb data sets with create_data.sh
mkdir caffe/data/CustomDataset
UPDATE labelmap.prototxt before create_data.sh  
/home/deek/deep/ssd_caffe_ref/caffe/data/CustomDataset/create_data.sh  
NOTE, this is where the docker bvlc/caffe:gpu fails, no LabelMap()
create_annoset.py, caffe_pb2.LabelMap()

GET create_data.sh FROM Reference Links below 
dataset_name="ConeLabels" #Edit this to your dataset name. Don't change anything else
data_root_dir="$HOME/MyDataset/$dataset_name"
mapfile="$data_root_dir/`labelmap_voc_cone.prototxt`"

export HOME=/home/deek if in docker bash

OUTPUTS in data_root_dir = /home/deek/MyDataset/ConeLabels
data_root_dir/ConeLabels/lmdb/`ConeLabels_test_lmdb`/, `ConeLabels_train_lmdb`/

QUESTION: How do the lmdbs work as my image data set gets very large?
It is all of the image data files smashed into one file. Inefficient disk space usage.
Do lmdb files solve the cpu RAM issue when training? (batch generator)

## MobileNet-SSD repo for training example
cd caffe/examples/
git clone https://github.com/chuanqi305/MobileNet-SSD

REMOVE MobileNet-SSD symlink x 3  
ln -s /home/deek/MyDataset/ConeLabels/labelmap_voc_cone.prototxt deep/ssd_caffe_ref/caffe/examples/MobileNet-SSD/labelmap.prototxt  
ln -s /home/deek/MyDataset/ConeLabels/ConeLabels/lmdb/`ConeLabels_n21classes_trainval_lmdb` deep/ssd_caffe_ref/caffe/examples/MobileNet-SSD/trainval_lmdb  
ln -s /home/deek/MyDataset/ConeLabels/ConeLabels/lmdb/`ConeLabels_n21classes_test_lmdb` deep/ssd_caffe_ref/caffe/examples/MobileNet-SSD/test_lmdb  

cd deep/ssd_caffe_ref/caffe/examples/MobileNet-SSD/
./gen_model.sh `21` # num classes  

creates MobileNet-SSD/example/MobileNetSSD_train,_test,_deploy.prototxt

## TRAINING ./train.sh
cd deep/ssd_caffe_ref/caffe/examples/MobileNet-SSD/  

Reduce batch size as needed (I got out of memory)
example/MobileNetSSD_train.prototxt, batch_size=12

solver_train.prototxt  
test_iter: 100 # test batch size??
test_interval: 50
snapshot: 500
snapshot_prefix: "snapshot/`mobilenetRepeat21c`"


./train.sh  

../../tools/extra/parse_log.py snapshot/aMN21cRepeat.log .  

../../build/tools/caffe train -solver="solver_train.prototxt" \
-weights="mobilenet_iter_73000.caffemodel" \
-gpu 0 2>&1 | tee -a snapshot/`aMN21cRepeat.log`

To resume a training from a solverstate
../../build/tools/caffe train -solver="solver_train.prototxt" \
-snapshot snapshot/mobilenet21c_iter_164.solverstate \
-gpu 0

## DEPLOY
cd deep/ssd_caffe_ref/caffe/examples/MobileNet-SSD/  
merge_bn.py
caffe_root = '/home/deek/deep/ssd_caffe_ref/caffe/'

python merge_bn.py --model example/n2classes/MobileNetSSD_deploy.prototxt --weights snapshot/mobilenet_iter_18000.caffemodel

rename no_bn.caffe_model and no_bn.prototxt and move to deploy subfolder

RENAME AND MOVE example/*.prototxt to subfolder as needed
(Will you change your model network prototxt? # classes the same?)

## Retraining Reference Links
https://tolotra.com/2018/09/15/how-to-retrain-ssd-mobilenet-for-real-time-object-detection-using-a-raspberry-pi-and-movidius-neural-compute-stick/#comment-71, 

https://github.com/tolotrasamuel/real-time-object-detection-retraining-ncs-raspberry-pi/tree/master/data/CustomDataset, 

http://shengshuyang.github.io/A-step-by-step-guide-to-Caffe.html, 

## Software setup: caffe, opencv


## Software setup Reference Links
http://www.voidcn.com/article/p-qzjrzdra-bkq.html
