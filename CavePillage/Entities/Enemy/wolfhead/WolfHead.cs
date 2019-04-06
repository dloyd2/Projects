using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WolfHead : Enemy
{
    [SerializeField] WolfHeadMovement move;
    bool throwing = false;
    GameObject knife;
    GameObject thrown;
    float knifeMax = 2f;
    float knifeThrowTimer = 2f;
    float throwMax = 0.3f;
    float throwTimer = 0.3f;
    int numThrows = 3;
    float throwForceX, throwForceY;
    
    void Awake()
    {
        base.Awake();
        startingHealth = 6;
        defense = 2;
        type = EnemyType.WOLFHEAD;
        remainingHealth = startingHealth;
        timeBetweenAttacks = 3.5f;
        attackDamage = 6;
        chanceToDrop = 60f;
        score = 25;
        SpawnManager.ChangeNumEnemies(1);
        knife = Resources.Load("Weapons/throwingKnife") as GameObject;
    }
    void startThrow()
    {
        move.SetMove(false);
        move.SetAttacking(true);
        throwing = true;
        throwTimer = throwMax;
    }
    void throwKnife()
    {
        
            if(numThrows > 0)
            {
                numThrows--;
                float diffInY = player.transform.position.y-gameObject.transform.position.y;
                float diffInX = player.transform.position.x-gameObject.transform.position.x;
                float rotateAmount = 90*(diffInY/8);
                if(diffInX < 0) rotateAmount  = (180-rotateAmount);
                
                thrown = Instantiate(knife);
                thrown.transform.position = gameObject.transform.position;
                thrown.transform.Rotate(0, 0, rotateAmount, Space.World);
                throwForceX = 500;
                throwForceY = 100*(diffInY);
                if(player.transform.position.x-gameObject.transform.position.x < 0){throwForceX *= -1;}
                thrown.GetComponent<Rigidbody2D>().AddForce(new Vector2(throwForceX, throwForceY));
                throwTimer = throwMax;
            }
        
        else if(numThrows <= 0)
        {
            knifeThrowTimer = knifeMax;
            move.SetMove(true);
            move.SetAttacking(false);
            throwing = false;
            numThrows = 3;
        }
        
    }
    // Update is called once per frame
    void Update()
    {
        CheckStatus();
        if(!move.GetMoving())
        {
            knifeThrowTimer -= Time.deltaTime;
            if(knifeThrowTimer <= 0 && !throwing)
            {
                startThrow();
            }
        }
        if(throwing)
        {
            throwTimer -= Time.deltaTime;
            if(throwTimer <= 0)
            {
                throwKnife();
            }
        }
        
    }
}
