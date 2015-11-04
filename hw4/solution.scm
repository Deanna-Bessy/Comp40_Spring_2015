;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                           ;;
;; Problem A                                                                 ;;
;;                                                                           ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Takes an argument and runs a loop that results in a value based off of the
;; input, helper functions p g & h, and the expression e
(define f-functional (y)
	(letrec ((x e) 
		(p_loop 
			(lambda (x y)
				(if (p x y)
					(p_loop (g x y) y)
					x ))))
		(h (p_loop x y) y)
	)
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                           ;;
;; Problem 10                                                                ;;
;;                                                                           ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;
;; Part b ;;
;;;;;;;;;;;;

;; Takes a single non-empty list and uses foldr to find the max
(define max*(xs)
	(foldr max (car xs) xs)
)

;; Check cases where: List is all different numbers
;;                    List is all the same number
;;                    Only one element is in list       
(check-expect (max* '(1 666 2 3 78 )) 666)
(check-expect (max* '(1 1 1 1 1)) 1)
(check-expect (max* '(666)) 666)

;;;;;;;;;;;;
;; Part c ;;
;;;;;;;;;;;;

;; Takes a single non-empty list and uses foldr to find the gcd
(define gcd*(xs)
	(foldr gcd (car xs) (cdr xs))	
)
;;Check cases where: Only testing general case. We know gcd and foldr work so
;;                   no reason to overtest a function that's in the same 
;;                   general form as b.	
(check-expect (gcd* '(26 104)) 26)

;;;;;;;;;;;;
;; Part d ;;
;;;;;;;;;;;;

;; Takes a single non-empty list and uses foldr to find the lcm
(define lcm*(xs)
	(foldr lcm (car xs) (cdr xs))	
)

;;Check cases where: Again, only general. Let's not beat a dead horse.
(check-expect (lcm* '( 13 26 52 104)) 104)

;;;;;;;;;;;;
;; Part e ;;
;;;;;;;;;;;;

;; Takes a single non-empty list and uses foldr to sum all elements
(define sum(xs)
	(foldr + (car xs) (cdr xs))
)

;; Check cases where: Only general. Foldr and + still work. I hope.
(check-expect (sum '(300 38 273 25 9 21)) 666)

;;;;;;;;;;;;
;; Part f ;;
;;;;;;;;;;;;

;; Takes a single non-empty list and uses foldr to find the product of all
;; the elements.
(define product(xs)
	(foldr * (car xs) (cdr xs))
)
;; Check cases where: Only general. Foldr and * still work. Let's hope 
;; the computer gnomes didn't change their meaning since I last checked.
(check-expect (product '(111 6)) 666)

;;;;;;;;;;;;
;; Part g ;;
;;;;;;;;;;;;

;; Takes two lists and uses foldr to append the second to the first.
(define append (xs ys)
	(foldr cons ys xs)
)

;; Check cases where: Appending two nonempty lists
;;                    Appending two empty lists
;;                    Appending one empty and one nonempty list
(check-expect (append '(1 2 3) '(4 5 6)) '(1 2 3 4 5 6))
(check-expect (append '() '()) '())
(check-expect (append '() '(1 2 3)) '(1 2 3))

;;;;;;;;;;;;
;; Part i ;;
;;;;;;;;;;;;

;; Reverses a list
(define reverse (xs)
		(foldl cons '() xs)
)

;; Check cases where: Reverse general list
;;                    Reverse empty list
;;                    Reverse single element list
(check-expect (reverse '(1 2 3 4 5)) '(5 4 3 2 1))
(check-expect (reverse '()) '())
(check-expect (reverse '(666)) '(666))

;;;;;;;;;;;;
;; Part j ;;
;;;;;;;;;;;;

;; Insert from book chunk 86b
(define insert (x xs)
	(if (null? xs)
		(list1 x)
		(if (< x (car xs))
			(cons x xs)
			(cons (car xs) (insert x (cdr xs)))	
		)
	)
)

;; Takes unordered list and orders it using foldr and insert
(define insertion-sort (xs)
		(foldr insert '() xs)
)

;; Check cases where: Sort general unordered list
;;                    Sort empty list
;;                    No other test cases: insert was given and works
(check-expect (insertion-sort '(66 6 6666 666 66)) '(6 66 66 666 6666))
(check-expect (insertion-sort '()) '())


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                           ;;
;; Problem 11                                                                ;;
;;                                                                           ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;
;; length ;;
;;;;;;;;;;;;

;;Uses foldr to calculate the length of a list
(define length (xs)
	(foldr (lambda (x y) (+ y 1)) 0 xs)
)

;; Test cases where: List is empty
;;                   List is not empty
(check-expect (length '()) 0)
(check-expect (length '(5 3 7 2)) 4)

;;;;;;;;;
;; map ;;
;;;;;;;;;

;; Uses foldr to implement map
(define map (f xs)
	(if (null? xs) 
		'()
		(foldr (lambda (x xs) (cons (f x) xs) ) '() xs)
	)
)

;; Test cases where: Adding 1 to all elements of list (nonempty list)
;;                   Adding 1 to all elements of list (empty list)	 
(check-expect (map (lambda (x) (+ x 1)) '(1 2 3)) '(2 3 4))
(check-expect (map (lambda (x) (+ x 1)) '()) '())

;;;;;;;;;;;;
;; filter ;;
;;;;;;;;;;;;

;; Uses foldr to implement filter
(define filter (f xs)
	(foldr (lambda (x xs) (if (f x) xs (cons x xs)) ) '() xs)
)

;; Check cases where: List has elements to filter
;;                    List has no elements to filter
;;                    List is empty 
(check-expect (filter (lambda (x) (= x 6)) '(1 2 6 3 4 6 5 6)) '(1 2 3 4 5))
(check-expect (filter (lambda (x) (= x 6)) '(666 66 666)) '(666 66 666))
(check-expect (filter (lambda (x) (= x 6)) '()) '())

;;;;;;;;;;;;;
;; exists? ;;
;;;;;;;;;;;;;

;; Uses foldr to check if there exists an element in the list that satisfies
;; the predicate
(define exists? (f xs)
	(foldr (lambda (x b) (if (f x) #t b)) #f xs)
)
;; Check cases where: Element does exist
;;                    Element does not exist
;;                    Empty list
(check-expect (exists? (lambda (x) (= x 6)) '(1 2 3 4 5 6)) #t)
(check-expect (exists? (lambda (x) (= x 6)) '(666 66 666)) #f)
(check-expect (exists? (lambda (x) (= x 6)) '()) #f)

;;;;;;;;;;
;; all? ;;
;;;;;;;;;;

;; Uses foldr to check if all elements of the list satisfy the predicate
(define all? (f xs)
	(if (null? xs) #f
		(foldr (lambda (x b) (if (f x) 
							(if b #t #f) #f)) #t xs))
)

;; Check cases where: Only last element doesn't satisfy predicate
;;                    Only first element doesn't satisfy predicate
;;                    List is one element, satisfies predicate
;;                    List is empty
(check-expect (all? (lambda (x) (= x 6)) '(6 6 6 2)) #f)
(check-expect (all? (lambda (x) (= x 6)) '(2 6 6 6)) #f)
(check-expect (all? (lambda (x) (= x 6)) '(6 6 6)) #t)
(check-expect (all? (lambda (x) (= x 6)) '(6)) #t)
(check-expect (all? (lambda (x) (= x 6)) '()) #f)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                           ;;
;; Problem 16                                                                ;;
;;                                                                           ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;
;; part a ;;
;;;;;;;;;;;;

;; Creates the emptyset as a starting point to create sets for testing
(val emptyset (lambda (x) #f))

;; From exercise transcript 170a, member? function for testing 
(define member? (x s) (s x)) 

;; Adds element x to the set represented by the function s
(define add-element (x s)
	(lambda (y) (or (= y x) (s y)))
)

;; Check cases where: Element has not yet been added
;;					  Single element has been added
;;                    Multiple elements have been added
(check-expect (member? 2 (add-element 1 emptyset)) #f)
(check-expect (member? 2 (add-element 2 emptyset)) #t)
(check-expect (member? 3 (add-element 1 (add-element 3 emptyset))) #t)
(check-expect (member? 1 (add-element 1 (add-element 3 emptyset))) #t)

;; Creates a set that is the union of two given sets
(define union (s z)
	(lambda (y) (or (s y) (z y)))
)

;; For testing purposes. Create two sets to test functions that have two sets 
;;                       as arguments.
(val 2_3_4 (add-element 4 (add-element 3 (add-element 2 emptyset))))
(val 5_1_7 (add-element 5 (add-element 1 (add-element 7 emptyset))))

;; Check cases where: Element from first set should be in union
;;                    Element from second set should be in union
;;                    Element from neither set should not be in the union
(check-expect (member? 2 (union 5_1_7 2_3_4)) #t)
(check-expect (member? 1 (union 5_1_7 2_3_4)) #t)
(check-expect (member? 10 (union 5_1_7 2_3_4)) #f)

;; Creates a set containing all elements that are in first list and not in 
;; the second
(define diff (s z)
	(lambda (y) (and (s y) (not (z y))))
)

;; Check cases where: Element in fist set only should be in diff
;;                    Element in both sets should not be in diff
;;                    Element in second set only should not be in diff
;;                    Element in neither set should not be in diff                
(check-expect (member? 2 (diff 2_3_4 (add-element 1 emptyset))) #t)
(check-expect (member? 4 (diff 2_3_4 (add-element 4 emptyset))) #f)
(check-expect (member? 5 (diff 2_3_4 5_1_7)) #f)
(check-expect (member? 10 (diff 2_3_4 5_1_7)) #f)

;; Creates a set containing all elements that are both in first list and
;; the second list
(define inter (s z)
	(lambda (y) (and (s y) (z y)))
)

;; Check cases where: Element in first set only should not be in inter
;;                    Element in both sets should be in inter
;;                    Element in second set only should not be in inter
;;                    Element in neither set should not be in inter                
(check-expect (member? 2 (inter 2_3_4 (add-element 1 emptyset))) #f)
(check-expect (member? 4 (inter 2_3_4 (add-element 4 emptyset))) #t)
(check-expect (member? 5 (inter 2_3_4 5_1_7)) #f)
(check-expect (member? 10 (inter 2_3_4 5_1_7)) #f)

;;;;;;;;;;;;
;; part b ;;
;;;;;;;;;;;;

;; Creates set containing set operations that use eqfun in place of = 
(val mk-set-ops (lambda (eqfun)
	(list6  
		(lambda (x) #f) ;emptyset
		(lambda (x s) (s x)) ;member?
		(lambda (x s) (lambda (y) (or (eqfun y x) (s y)))) ;add-element
		(lambda (s z) (lambda (y) (or (s y) (z y))));union
		(lambda (s z) (lambda (y) (and (s y) (z y)))) ;inter 
		(lambda (s z) (lambda (y) (and (s y) (not (z y))))) ;diff
	))
)

;; Creates set of operations, passing in = function as eqfun
(val list-of-al-ops (mk-set-ops =))

;; Extracts functions from mk-set-ops
(val emptyset (car list-of-al-ops))
(val member? (car (cdr list-of-al-ops)))
(val add-element (car (cdr (cdr list-of-al-ops))))
(val union (car (cdr (cdr (cdr list-of-al-ops)))))
(val inter (car (cdr (cdr (cdr (cdr list-of-al-ops))))))
(val diff (car (cdr (cdr (cdr (cdr (cdr list-of-al-ops)))))))

;; For testing purposes. Create two sets to test functions that have two sets 
;;                       as arguments.
(val 2_3_4 (add-element 4 (add-element 3 (add-element 2 emptyset))))
(val 5_1_7 (add-element 5 (add-element 1 (add-element 7 emptyset))))

;; Check cases where: Check one true and one false case for each newly 
;; 					  redefined polymorphic function
(check-expect (member? 2 (add-element 1 emptyset)) #f)
(check-expect (member? 2 (add-element 2 emptyset)) #t)
(check-expect (member? 1 (union 5_1_7 2_3_4)) #t)
(check-expect (member? 10 (union 5_1_7 2_3_4)) #f)
(check-expect (member? 2 (diff 2_3_4 (add-element 1 emptyset))) #t)
(check-expect (member? 4 (diff 2_3_4 (add-element 4 emptyset))) #f)
(check-expect (member? 2 (inter 2_3_4 (add-element 1 emptyset))) #f)
(check-expect (member? 4 (inter 2_3_4 (add-element 4 emptyset))) #t)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                           ;;
;; Problem 23                                                                ;;
;;                                                                           ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Given a formula following the 3 laws on page 179, finds values for the 
;; symbols that makes the formula evaluate to true, and returns them as 
;; key-value pairs in an association list
(define make-formula-true (f fail succ)
	(letrec (
		(make-any (lambda (formulas bool cur fail succeed)
			(if (null? formulas) (fail)
				(make-formula (car formulas) bool cur (lambda () (make-any (cdr formulas) bool cur fail succeed )) succeed)))
		)
		(make-all (lambda (formulas bool cur fail succeed)
			(begin  (if (null? formulas) (succeed cur fail)
				(make-formula (car formulas) bool cur fail (lambda (cur fail) (make-formula (cadr formulas) bool cur fail succeed))))))
		)
		(make-formula (lambda (formulas bool cur fail succeed)
			(if (symbol? formulas)

				(if (null? (find formulas cur)) 
					(succeed (bind formulas bool cur) fail)
					(fail))
				(if (= (car formulas) 'not) 
					(make-formula (cadr formulas) (not bool) cur fail succeed)	
	    			(if (= (car formulas) 'and)
	    				(make-all (cdr formulas) bool cur fail succeed)
	    					(if (= (car formulas) 'or)
	    						(make-any (cdr formulas) bool cur fail succeed)
	    						(make-formula (car formulas) bool cur fail succeed))))))
		)
	)
	(if (symbol? f) 
		(succ (bind f #t '()) (lambda (g) g))
		(make-formula f #t '() fail succ)))
)