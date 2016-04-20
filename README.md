[ ![Codeship Status for 3Hren/metrics](https://codeship.com/projects/7217d2a0-5880-0133-4f33-1ebb29223a5a/status?branch=master)](https://codeship.com/projects/109692)

### Advantages

- No lock-free bullshit (yet, until required)
- Tagged metric names

### Limitations

- Discrete mathematic for histograms - no support yet

### Planning

### References
- http://www.johndcook.com/blog/standard_deviation/ - Accurately computing running variance.

{
  "rate": [0.9, 1.0, 2.5],
  "timings": {
    ...
    "0.95": ...,
  },
  // Maybe allow to c§onfigure through manifest?
  "reverse_timings": {
    "1": 200,
    "2": 100,
    "5": 50,
    "10": 20,
    "20": 4,
    "50": 2,
    "100": 1
    "1000": 1,
    "10000": 10   
  }
}
