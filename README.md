# FlexPipe

## What It Is

**FlexPipe**, as a library, presents a framework that enables flexibly distributed pipelines for millisecond-scale and real-time applications.
Originally, we published this framework for interactive multimedia AR/VR applications as [FleXR](https://github.com/gt-flexr/FleXR). For ease of use, we wrap our framework as a directly usable library and name it as FlexPipe.
Currently, FlexPipe supports Linux, and it is tested in Ubuntu 18, 20, 22.

## What It Offers
As a pipeline framework, it requires each functionality to be implemented as a separate compute kernel, and the pipeline is configured by connecting the implemented kernels.
While there are many libraries for this, FlexPipe is novel in the below aspects.

1. **Implement Once, Use Flexibly**
    - Once the kernel is implemented by the FlexPipe's kernel template, it can be flexibly configured *without any code changes* for connection types (local/remote), connection dependencies (blocking/non-blocking), and remote protocols (TCP/UDP-based RTP).
    - With given kernels, it is possible to create diverse pipelines with different distributed topologies. FlexPipe supports dynamic branching the kernel port; even when a kernel has one output connection port, it can be duplicated for multiple downstream kernels of different connection semantics.
2. **High Performance**
    - While presenting the flexible configuration, the FlexPipe runtime ensures the millisecond-scale performance. When the kernels are locally connected under its runtime, the data communication overhead is minimal even for the large multimedia data.
3. **Compatibility and Extensibility**
    - FlexPipe supports the shared-memory-based IPC. Thus, the other processes can be seamlessly integrated as a part of the pipeline; we integrated the AR/VR game applications developed by Unity/Unreal Engine along with the perception kernels in [1].
    - The kernel design of FlexPipe is highly extensible. By extending its port interface, the new transmission protocol can be easily added, enabling the different communication protocols' impact on the application pipeline.

## Installation
```
git clone https://github.com/gt-flexr/FlexPipe.git
cd FlexPipe

# Dependencies
./install_dependencies.sh  # to install all at once. Or, it is also possible to install one by one manually.

# Install FlexPipe
mkdir build && cd build
cmake .. & make -j$(nproc)
sudo make install
```

Now, you are ready to go. For how to use FlexPipe, please check [**our tutorial in Wiki**](https://github.com/gt-flexr/FlexPipe/wiki)!




## Citation
[1] Heo, J., Bhardwaj, K., & Gavrilovska, A. (2023, June). [FleXR: A System Enabling Flexibly Distributed Extended Reality](https://dl.acm.org/doi/abs/10.1145/3587819.3590966). In Proceedings of the 14th Conference on ACM Multimedia Systems (pp. 1-13).

BibTex
```
@inproceedings{heo2023flexr,
  title={FleXR: A System Enabling Flexibly Distributed Extended Reality},
  author={Heo, Jin and Bhardwaj, Ketan and Gavrilovska, Ada},
  booktitle={Proceedings of the 14th Conference on ACM Multimedia Systems},
  pages={1--13},
  year={2023}
}
```