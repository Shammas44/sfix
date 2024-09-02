from setuptools import setup, find_packages

setup(
    name="sfix",
    version="0.1",
    description="SFIX protocol wrapper for python",
    author="Sébastien Traber",
    install_requires=[],
    packages=find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.6",
)
