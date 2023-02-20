**Тестовое задание на позицию C++ разработчика** 

Задание нацелено на оценку уровня владения языком программирования, математического анализа или вычислительной геометрии и способности разобраться в новом материале при необходимости. 

Задание составлено на английском языке, т.к. большая часть технической информации в будущем будет доступна именно на нем. 

Пожалуйста, внимательно прочтите все обязательные требования! Вы также можете выполнить дополнительные задания (под «звездочкой»), это может стать Вашим преимуществом перед другими кандидатами. 

Ответ следует направить на адрес в виде ссылки на проект на github, который должен включать исходный код (\*.h, \*.cpp), а также проектные файлы. 

\----------------------------------------------- 

Design a small program in C++ that would implement support of 3D curves hierarchy. 

1. Support a few types of 3D geometric curves – circles, ellipses and 3D helixes. (Simplified definitions are below). Each curve should be able to return a 3D point and a first derivative (3D vector) per parameter *t* along the curve. 
1. Populate a container (e.g. vector or list) of objects of these types created in random manner with random parameters. 
1. Print coordinates of points and derivatives of all curves in the container at t=PI/4. 
1. Populate a second container that would contain only circles from the first container. Make sure the second container shares (i.e. not clones) circles of the first one, e.g. via pointers. 
1. Sort the second container in the ascending order of circles’ radii. That is, the first element has the smallest radius, the last - the greatest. 
1. Compute the total sum of radii of all curves in the second container. 

(\*) Additional optional requirements: 

7. Split implementation into a library of curves (.dll or .so) and executable which uses API of this library. 
7. Implement computation of the total sum of radii using parallel computations (e.g. OpenMP or Intel TBB library). 

Requirements to the implementation: 

1. The implementation must use virtual methods. 
1. Has neither explicit memory deallocation nor memory leaks. 
1. Curves must be physically correct (e.g. radii must be positive). 
1. Containers and sorting must be implemented using STL (C++ Standard Template Library). 
1. The implementation may use constructs of C++11 or higher. 
1. The code must compile with any compiler of your choice (gcc, Visual C++, etc). 

Curve definitions: 

- All curves are parametrically defined, i.e. a point is calculated using some C(t) formula. 
- Circle is planar in the plane XoY (i.e. all Z-coordinates are 0) and is defined by its radius. 
- Ellipse is planar in the plane XoY and is defined by its two radii, along X and Y axes. 
- Helix is spatial and is defined by its radius and step (see the figure below). It takes 2 \* PI in parametric space to make a round, i.e. any point on helix satisfies the condition C(t + 2\*PI) = C(t) + {0, 0, step}. 