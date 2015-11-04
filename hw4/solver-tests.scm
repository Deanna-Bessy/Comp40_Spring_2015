(val f1 '(and y (not (or y y))))
(val s1 'no-solution)

(val f2 '(not (not (and (or z y) x))))
(val s2 '((z #t) (x #t)))

(val f3 '(and p (not (not (or (or z (and r y)) x)))))
(val s3 '((p #t) (z #t)))