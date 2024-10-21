<div align="center">
    SQLITE FROM SCRATCH
</div>

<span style="color:red;">WARNING: </span> <span>The project is still under implementation phase</span>

<p>
    This is an implementation of sqlite from scratch. The implementation includes studying the sqlite architecture from: <a href="https://www.sqlite.org/arch.html">https://www.sqlite.org/arch.html</a> and then re-implmenting in C.
    Inspired from: https://cstack.github.io/db_tutorial/
</p>

<h4>What the database can do so far:</h4>
<ul>
    <li>It can store user records in a ".db" file</li>
    <li>It can read from a .db file</li>
</ul>

<h4>Drawbacks: </h4>
<ul>
    <li>It can not insert data in sorted order</li>
    <li>It has O(n) retrieval time</li>
    <li>A bug in the code results in error at 28 entries even though in theory the database should be able to insert more approximately 1400 entries</li>
</ul>
