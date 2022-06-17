### **About project**

Quiz - is a simple application that allows you to solve quizzes.

Here you can test [demo](https://jacek13.github.io/) of the application in the WebAssembly version in your browser. Note that this version does not support the full functionality of the PC version (QT WebAssambly limitations).

---

### **How to load questions**

We can pass files with questions from the local machine or use an internet connection. Then enter the URL with the hosted quiz into the application

---

### **File format**

Quizzes are stored in .json format. Below is an example file:

```json
{
   "categories":[
      {
         "name":"First category",
         "questions":[
            {
               "question":"Select best programming language.",
               "answers":[
                  "C",
                  "C++",
                  "C#",
                  "Python"
               ],
               "values":[
                  true,
                  true,
                  true,
                  false
               ]
            },
            {
               "question":"Select fastest programming language",
               "answers":[
                  "Java",
                  "C#",
                  "C",
                  "Python"
               ],
               "values":[
                  false,
                  false,
                  true,
                  false
               ]
            }
         ]
      },
      {
         "name":"Second category",
         "questions":[
            {
               "question":"Select slowest programming language.",
               "answers":[
                  "Java",
                  "C++",
                  "C#",
                  "C"
               ],
               "values":[
                  true,
                  false,
                  false,
                  false
               ]
            }
         ]
      }
   ]
}
```