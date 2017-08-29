sudo docker build -t hl_cmake_ready .
MEMORY=4294967296
echo $MEMORY
sudo docker run -it --rm -p 80:80 -m=$MEMORY --ulimit memlock=$MEMORY:$MEMORY --net=host -v/home/dragoon/IdeaProjects/highload/data:/tmp/data -t hl_cmake_ready
