using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Skeleton : Enemy
{
    
    [SerializeField] SkeletonMovement move;
    GameObject bone;
    GameObject thrown;
    bool throwing = false;
    float boneMax = 0.6f;
    float boneTimer = 0.6f;
    float throwMax = 1f;
    float throwTimer = 0.1f;
    float throwForceX, throwForceY;
    
    public override void Awake()
    {
        base.Awake();
        startingHealth = 2;
        defense = 1;
        type = EnemyType.SKELETON;
        remainingHealth = startingHealth;
        timeBetweenAttacks = 1.5f;
        attackDamage = 2;
        chanceToDrop = 20f;
        score = 10;
        SpawnManager.ChangeNumEnemies(1);
        bone = Resources.Load("Weapons/bone") as GameObject;
    }
    
    void startThrow()
    {
        move.SetMove(false);
        move.SetAttacking(true);
        throwing = true;
        throwTimer = throwMax;
    }
    
    void throwBone()
    {
        boneTimer = boneMax;
        thrown = Instantiate(bone);
        thrown.transform.position = gameObject.transform.position;
        throwForceX = 480;
        throwForceY = 100*(player.transform.position.y-gameObject.transform.position.y)+100;
        if(player.transform.position.x-gameObject.transform.position.x < 0){throwForceX *= -1;}
        thrown.GetComponent<Rigidbody2D>().AddForce(new Vector2(throwForceX,throwForceY));
        throwing = false;
        move.SetMove(true);
        move.SetAttacking(false);
    }
    
    // Update is called once per frame
    void Update()
    {
        CheckStatus();
        if(!move.GetMoving())
        {
            boneTimer -= Time.deltaTime;
            if(boneTimer <= 0 && !throwing)
            {
                startThrow();
            }
        }
        if(throwing)
        {
            throwTimer -= Time.deltaTime;
            if(throwTimer <= 0)
            {
                throwBone();
            }
        }
    }
}
